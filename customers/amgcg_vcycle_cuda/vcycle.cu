#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <vector>
#include <cuda_runtime.h>
#include <cublas_v2.h>
#include <cusparse.h>

#if __GNUC__ && __linux__
#include <sys/ptrace.h>

[[noreturn]] static void cuerr() {
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) != 0)
        __builtin_trap();
    exit(EXIT_FAILURE);
}
#elif _WIN32 && _MSC_VER
#include <windows.h>

[[noreturn]] static void cuerr() {
    int debugger_present = 0;
    HANDLE process = GetCurrentProcess();
    CheckRemoteDebuggerPresent(process, &debugger_present);
    if (debugger_present) {
        __debugbreak();
    }
    exit(EXIT_FAILURE);
}
#else
[[noreturn]] static void cuerr() {
    exit(EXIT_FAILURE);
}
#endif

#define CHECK_CUDA(func)                                                       \
{                                                                              \
    cudaError_t status = (func);                                               \
    if (status != cudaSuccess) {                                               \
        printf("%s:%d: %s (%d): %s\n", __FILE__, __LINE__,                     \
               cudaGetErrorString(status), status, #func);                     \
        cuerr();                                                               \
    }                                                                          \
}

#define CHECK_CUSPARSE(func)                                                   \
{                                                                              \
    cusparseStatus_t status = (func);                                          \
    if (status != CUSPARSE_STATUS_SUCCESS) {                                   \
        printf("%s:%d: %s (%d): %s\n", __FILE__, __LINE__,                     \
               cusparseGetErrorString(status), status, #func);                 \
        cuerr();                                                               \
    }                                                                          \
}

#define CHECK_CUBLAS(func)                                                     \
{                                                                              \
    cublasStatus_t status = (func);                                            \
    if (status != CUBLAS_STATUS_SUCCESS) {                                     \
        printf("%s:%d: %s (%d): %s\n", __FILE__, __LINE__,                     \
               cublasGetStatusString(status), status, #func);                  \
        cuerr();                                                               \
    }                                                                          \
}

namespace {

struct Buffer {
    void *m_data;
    size_t m_cap;

    Buffer() noexcept : m_data(nullptr), m_cap(0) {
    }

    Buffer(Buffer &&that) noexcept : m_data(that.m_data), m_cap(that.m_cap) {
        that.m_data = nullptr;
        that.m_cap = 0;
    }

    Buffer &operator=(Buffer &&that) noexcept {
        if (this == &that) return *this;
        if (m_data)
            CHECK_CUDA(cudaFree(m_data));
        m_data = nullptr;
        m_data = that.m_data;
        m_cap = that.m_cap;
        that.m_data = nullptr;
        that.m_cap = 0;
        return *this;
    }

    ~Buffer() noexcept {
        if (m_data)
            CHECK_CUDA(cudaFree(m_data));
        m_data = nullptr;
    }

    void reserve(size_t new_cap) {
        if (m_cap < new_cap) {
            if (m_data)
                CHECK_CUDA(cudaFree(m_data));
            m_data = nullptr;
            CHECK_CUDA(cudaMalloc(&m_data, new_cap));
            m_cap = new_cap;
        }
    }

    size_t capacity() const noexcept {
        return m_cap;
    }

    void const *data() const noexcept {
        return m_data;
    }

    void *data() noexcept {
        return m_data;
    }
};

template <class T>
cudaDataType_t cudaDataTypeFor();

template <>
cudaDataType_t cudaDataTypeFor<int8_t>() {
    return CUDA_R_8I;
}

template <>
cudaDataType_t cudaDataTypeFor<uint8_t>() {
    return CUDA_R_8U;
}

template <>
cudaDataType_t cudaDataTypeFor<int16_t>() {
    return CUDA_R_16I;
}

template <>
cudaDataType_t cudaDataTypeFor<uint16_t>() {
    return CUDA_R_16U;
}

template <>
cudaDataType_t cudaDataTypeFor<int32_t>() {
    return CUDA_R_32I;
}

template <>
cudaDataType_t cudaDataTypeFor<uint32_t>() {
    return CUDA_R_32U;
}

template <>
cudaDataType_t cudaDataTypeFor<int64_t>() {
    return CUDA_R_64I;
}

template <>
cudaDataType_t cudaDataTypeFor<uint64_t>() {
    return CUDA_R_64U;
}

template <>
cudaDataType_t cudaDataTypeFor<nv_half>() {
    return CUDA_R_16F;
}

template <>
cudaDataType_t cudaDataTypeFor<nv_bfloat16>() {
    return CUDA_R_16BF;
}

template <>
cudaDataType_t cudaDataTypeFor<float>() {
    return CUDA_R_32F;
}

template <>
cudaDataType_t cudaDataTypeFor<double>() {
    return CUDA_R_64F;
}

template <class T>
struct Vec {
    T *m_data;
    size_t m_size;
    size_t m_cap;

    Vec() noexcept : m_data(nullptr), m_size(0), m_cap(0) {
    }

    Vec(Vec &&that) noexcept : m_data(that.m_data), m_size(that.m_size), m_cap(that.m_cap) {
        that.m_data = nullptr;
        that.m_size = 0;
        that.m_cap = 0;
    }

    Vec &operator=(Vec &&that) noexcept {
        if (this == &that) return *this;
        if (m_data)
            CHECK_CUDA(cudaFree(m_data));
        m_data = nullptr;
        m_data = that.m_data;
        m_size = that.m_size;
        m_cap = that.m_cap;
        that.handle = nullptr;
        that.m_data = nullptr;
        that.m_size = 0;
        that.m_cap = 0;
        return *this;
    }

    void swap(Vec &that) noexcept {
        std::swap(m_data, that.m_data);
        std::swap(m_size, that.m_size);
        std::swap(m_cap, that.m_cap);
    }

    ~Vec() noexcept {
        if (m_data)
            CHECK_CUDA(cudaFree(m_data));
        m_data = nullptr;
    }

    void resize(size_t new_size) {
        bool change = m_cap < new_size;
        if (change) {
            if (m_data)
                CHECK_CUDA(cudaFree(m_data));
            m_data = nullptr;
            CHECK_CUDA(cudaMalloc(&m_data, sizeof(T) * new_size));
            m_cap = new_size;
        }
        if (m_size != new_size || change) {
            m_size = new_size;
        }
    }

    void assign(T const *data, size_t size) {
        resize(size);
        CHECK_CUDA(cudaMemcpy(m_data, data, sizeof(T) * size, cudaMemcpyHostToDevice));
    }

    void store(T *data) const {
        CHECK_CUDA(cudaMemcpy(data, m_data, sizeof(T) * size(), cudaMemcpyDeviceToHost));
    }

    size_t size() const noexcept {
        return m_size;
    }

    T const *data() const noexcept {
        return m_data;
    }

    T *data() noexcept {
        return m_data;
    }
};

struct DnVec {
    cusparseDnVecDescr_t handle;

    operator cusparseDnVecDescr_t() const noexcept {
        return handle;
    }

    DnVec() noexcept : handle(0) {}

    template <class T>
    DnVec(Vec<T> &v) {
        CHECK_CUSPARSE(cusparseCreateDnVec(&handle, v.size(), v.data(), cudaDataTypeFor<T>()));
    }

    DnVec(DnVec &&that) noexcept : handle(that.handle) {
        that.handle = nullptr;
    }

    DnVec &operator=(DnVec &&that) noexcept {
        if (this == &that) return *this;
        if (handle)
            CHECK_CUSPARSE(cusparseDestroyDnVec(handle));
        handle = that.handle;
        that.handle = nullptr;
        return *this;
    }

    ~DnVec() {
        if (handle)
            CHECK_CUSPARSE(cusparseDestroyDnVec(handle));
    }
};

struct ConstDnVec {
    cusparseConstDnVecDescr_t handle;

    operator cusparseConstDnVecDescr_t() const noexcept {
        return handle;
    }

    ConstDnVec() noexcept : handle(0) {}

    template <class T>
    ConstDnVec(Vec<T> const &v) {
        CHECK_CUSPARSE(cusparseCreateConstDnVec(&handle, v.size(), v.data(), cudaDataTypeFor<T>()));
    }

    ConstDnVec(ConstDnVec &&that) noexcept : handle(that.handle) {
        that.handle = nullptr;
    }

    ConstDnVec &operator=(ConstDnVec &&that) noexcept {
        if (this == &that) return *this;
        if (handle)
            CHECK_CUSPARSE(cusparseDestroyDnVec(handle));
        handle = that.handle;
        that.handle = nullptr;
        return *this;
    }

    ~ConstDnVec() {
        if (handle)
            CHECK_CUSPARSE(cusparseDestroyDnVec(handle));
    }
};

template <class T>
struct CSR {
    Vec<T> data;
    Vec<int> indices;
    Vec<int> indptr;
    size_t nrows;
    size_t ncols;
    size_t numnonz;

    CSR() noexcept : nrows(0), ncols(0), numnonz(0) {}

    void assign(T const *datap, size_t ndat, int const *indicesp, size_t nind, int const *indptrp, size_t nptr, size_t rows, size_t cols, size_t nnz) {
        data.resize(ndat);
        CHECK_CUDA(cudaMemcpy(data.data(), datap, data.size() * sizeof(T), cudaMemcpyHostToDevice));
        indices.resize(nind);
        CHECK_CUDA(cudaMemcpy(indices.data(), indicesp, indices.size() * sizeof(int), cudaMemcpyHostToDevice));
        indptr.resize(nptr);
        CHECK_CUDA(cudaMemcpy(indptr.data(), indptrp, indptr.size() * sizeof(int), cudaMemcpyHostToDevice));
        nrows = rows;
        ncols = cols;
        numnonz = nnz;
    }
};

struct SpMat {
    cusparseSpMatDescr_t handle;

    operator cusparseSpMatDescr_t() const noexcept {
        return handle;
    }

    SpMat() noexcept : handle(0) {}

    template <class T>
    SpMat(CSR<T> &m) {
        CHECK_CUSPARSE(cusparseCreateCsr(&handle, m.nrows, m.ncols, m.numnonz,
                                         m.indptr.data(), m.indices.data(), m.data.data(),
                                         CUSPARSE_INDEX_32I, CUSPARSE_INDEX_32I,
                                         CUSPARSE_INDEX_BASE_ZERO, cudaDataTypeFor<T>()) );
    }

    SpMat(SpMat &&that) noexcept : handle(that.handle) {
        that.handle = nullptr;
    }

    SpMat &operator=(SpMat &&that) noexcept {
        if (this == &that) return *this;
        if (handle)
            CHECK_CUSPARSE(cusparseDestroySpMat(handle));
        handle = that.handle;
        that.handle = nullptr;
        return *this;
    }

    ~SpMat() {
        if (handle)
            CHECK_CUSPARSE(cusparseDestroySpMat(handle));
    }
};

struct ConstSpMat {
    cusparseConstSpMatDescr_t handle;

    operator cusparseConstSpMatDescr_t() const noexcept {
        return handle;
    }

    ConstSpMat() noexcept : handle(0) {}

    template <class T>
    ConstSpMat(CSR<T> const &m) {
        CHECK_CUSPARSE(cusparseCreateConstCsr(&handle, m.nrows, m.ncols, m.numnonz,
                                              m.indptr.data(), m.indices.data(), m.data.data(),
                                              CUSPARSE_INDEX_32I, CUSPARSE_INDEX_32I,
                                              CUSPARSE_INDEX_BASE_ZERO, cudaDataTypeFor<T>()) );
    }

    ConstSpMat(SpMat &&that) noexcept : handle(that.handle) {
        that.handle = nullptr;
    }

    ConstSpMat &operator=(ConstSpMat &&that) noexcept {
        if (this == &that) return *this;
        if (handle)
            CHECK_CUSPARSE(cusparseDestroySpMat(handle));
        handle = that.handle;
        that.handle = nullptr;
        return *this;
    }

    ~ConstSpMat() {
        if (handle)
            CHECK_CUSPARSE(cusparseDestroySpMat(handle));
    }
};

struct Kernels {
    cublasHandle_t cublas;
    cusparseHandle_t cusparse;

    Kernels() {
        CHECK_CUSPARSE(cusparseCreate(&cusparse));
        CHECK_CUBLAS(cublasCreate_v2(&cublas));
    }

    Kernels(Kernels &&) = delete;

    ~Kernels() {
        CHECK_CUSPARSE(cusparseDestroy(cusparse));
        CHECK_CUBLAS(cublasDestroy_v2(cublas));
    }

    // out = alpha * A@x + beta * out
    void spmv(Vec<float> &out, float const &alpha, CSR<float> const &A, Vec<float> const &x, float const &beta, Buffer &buffer) {
        assert(out.size() == A.nrows);
        size_t bufSize = 0;
        ConstSpMat dA(A);
        ConstDnVec dx(x);
        DnVec dout(out);
        CHECK_CUSPARSE(cusparseSpMV_bufferSize(cusparse, CUSPARSE_OPERATION_NON_TRANSPOSE,
                                               &alpha, dA, dx, &beta,
                                               dout, cudaDataTypeFor<float>(),
                                               CUSPARSE_SPMV_ALG_DEFAULT, &bufSize));
        buffer.reserve(bufSize);
        CHECK_CUSPARSE(cusparseSpMV(cusparse, CUSPARSE_OPERATION_NON_TRANSPOSE,
                                    &alpha, dA, dx, &beta,
                                    dout, cudaDataTypeFor<float>(),
                                    CUSPARSE_SPMV_ALG_DEFAULT, buffer.data()));
    }

    // dst = src + alpha * dst
    void axpy(Vec<float> &dst, float const &alpha, Vec<float> const &src) {
        assert(dst.size() == src.size());
        CHECK_CUBLAS(cublasSaxpy_v2(cublas, dst.size(), &alpha, src.data(), 1, dst.data(), 1));
    }

    void zero(Vec<float> &dst) {
        CHECK_CUDA(cudaMemset(dst.data(), 0, dst.size() * sizeof(float)));
    }

    void copy(Vec<float> &dst, Vec<float> const &src) {
        dst.resize(src.size());
        CHECK_CUDA(cudaMemcpy(dst.data(), src.data(), src.size() * sizeof(float), cudaMemcpyDeviceToDevice));
    }

    // dst = alpha * x
    void scal(Vec<float> &dst, float const &alpha, Vec<float> const &x) {
        copy(dst, x);
        CHECK_CUBLAS(cublasSscal_v2(cublas, dst.size(), &alpha, dst.data(), 1));
    }

    // x = A^{-1} b
    void spsolve(Vec<float> &x, CSR<float> const &A, Vec<float> const &b) {
        throw;
    }
};

struct MGLevel {
    CSR<float> A;
    CSR<float> R;
    CSR<float> P;
    Vec<float> residual;
    Vec<float> b;
    Vec<float> x;
    Vec<float> h;
    Vec<float> outh;
};

struct VCycle : Kernels {
    std::vector<MGLevel> levels;
    size_t nlvs;
    std::vector<float> coefficients;
    Vec<float> init_x;
    Vec<float> init_b;
    Buffer buf1;
    Buffer buf2;
    Buffer buf3;
    Buffer buf4;
    Buffer buf5;

    void setup(size_t numlvs) {
        if (levels.size() < numlvs) {
            levels.resize(numlvs);
        }
        nlvs = numlvs;
        coefficients.clear();
    }

    void set_lv_csrmat(size_t lv, size_t which, float const *datap, size_t ndat, int const *indicesp, size_t nind, int const *indptrp, size_t nptr, size_t rows, size_t cols, size_t nnz) {
        CSR<float> *mat = nullptr;
        if (which == 1) mat = &levels.at(lv).A;
        if (which == 2) mat = &levels.at(lv).R;
        if (which == 3) mat = &levels.at(lv).P;
        if (mat) {
            mat->assign(datap, ndat, indicesp, nind, indptrp, nptr, rows, cols, nnz);
        }
    }

    void set_coeff(float const *coeff, size_t ncoeffs) {
        coefficients.assign(coeff, coeff + ncoeffs);
    }

    void _smooth(int lv, Vec<float> &x, Vec<float> const &b) {
        copy(levels.at(lv).residual, b);
        spmv(levels.at(lv).residual, -1, levels.at(lv).A, x, 1, buf4); // residual = b - A@x
        scal(levels.at(lv).h, coefficients.at(0), levels.at(lv).residual); // h = c0 * residual


        for (int i = 1; i < coefficients.size(); ++i) {
            // h' = ci * residual + A@h
            copy(levels.at(lv).outh, levels.at(lv).residual);
            spmv(levels.at(lv).outh, 1, levels.at(lv).A, levels.at(lv).h, coefficients.at(i), buf5);

            // copy(levels.at(lv).h, levels.at(lv).outh);
            levels.at(lv).h.swap(levels.at(lv).outh);
        }

        axpy(x, 1, levels.at(lv).h); // x += h
    }

    void set_x_b(float const *x, float const *b, size_t n) {
        init_x.resize(n);
        init_b.resize(n);
        CHECK_CUDA(cudaMemcpy(init_x.data(), x, n * sizeof(float), cudaMemcpyHostToDevice));
        CHECK_CUDA(cudaMemcpy(init_b.data(), b, n * sizeof(float), cudaMemcpyHostToDevice));
    }

    void vcycle_down() {
        for (int lv = 0; lv < nlvs-1; ++lv) {
            Vec<float> &x = lv != 0 ? levels.at(lv - 1).x : init_x;
            Vec<float> &b = lv != 0 ? levels.at(lv - 1).b : init_b;
            _smooth(lv, x, b);

            copy(levels.at(lv).residual, b);
            spmv(levels.at(lv).residual, -1, levels.at(lv).A, x, 1, buf1); // residual = b - A@x

            levels.at(lv).b.resize(levels.at(lv).R.nrows);
            spmv(levels.at(lv).b, 1, levels.at(lv).R, levels.at(lv).residual, 0, buf2); // coarse_b = R@residual

            levels.at(lv).x.resize(levels.at(lv).b.size());
            zero(levels.at(lv).x);
        }
    }

    void vcycle_up() {
        for (int lv = nlvs-2; lv >= 0; --lv) {
            Vec<float> &x = lv != 0 ? levels.at(lv - 1).x : init_x;
            Vec<float> &b = lv != 0 ? levels.at(lv - 1).b : init_b;
            spmv(x, 1, levels.at(lv).P, levels.at(lv).x, 1, buf3); // x += P@coarse_x
            _smooth(lv, x, b);
        }
    }

    size_t get_coarsist_size() {
        auto const &this_b = levels.at(nlvs - 2).b;
        return this_b.size();
    }

    void get_coarsist_b(float *b) {
        auto const &this_b = levels.at(nlvs - 2).b;
        CHECK_CUDA(cudaMemcpy(b, this_b.data(), this_b.size() * sizeof(float), cudaMemcpyDeviceToHost));
    }

    void get_finest_x(float *x) {
        CHECK_CUDA(cudaMemcpy(x, init_x.data(), init_x.size() * sizeof(float), cudaMemcpyDeviceToHost));
    }

    void set_coarsist_x(float const *x) {
        auto const &this_b = levels.at(nlvs - 2).b;
        auto &this_x = levels.at(nlvs - 2).x;
        this_x.resize(this_b.size());
        CHECK_CUDA(cudaMemcpy(this_x.data(), x, this_x.size() * sizeof(float), cudaMemcpyHostToDevice));
    }

    void coarse_solve() {
        auto const &A = levels.at(nlvs - 1).A;
        auto &x = levels.at(nlvs - 2).x;
        auto const &b = levels.at(nlvs - 2).b;
        spsolve(x, A, b);
    }
};

}

static VCycle *fastmg = nullptr;

#if _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

extern "C" DLLEXPORT void fastmg_setup(size_t numlvs) {
    if (!fastmg)
        fastmg = new VCycle{};
    fastmg->setup(numlvs);
}

extern "C" DLLEXPORT void fastmg_set_coeff(float const *coeff, size_t ncoeffs) {
    fastmg->set_coeff(coeff, ncoeffs);
}

extern "C" DLLEXPORT void fastmg_set_lv_csrmat(size_t lv, size_t which, float const *datap, size_t ndat, int const *indicesp, size_t nind, int const *indptrp, size_t nptr, size_t rows, size_t cols, size_t nnz) {
    fastmg->set_lv_csrmat(lv, which, datap, ndat, indicesp, nind, indptrp, nptr, rows, cols, nnz);
}

extern "C" DLLEXPORT void fastmg_set_x_b(float const *x, float const *b, size_t n) {
    fastmg->set_x_b(x, b, n);
}

extern "C" DLLEXPORT void fastmg_vcycle_down() {
    fastmg->vcycle_down();
}

extern "C" DLLEXPORT void fastmg_vcycle_up() {
    fastmg->vcycle_up();
}

extern "C" DLLEXPORT size_t fastmg_get_coarsist_size() {
    return fastmg->get_coarsist_size();
}

extern "C" DLLEXPORT void fastmg_get_coarsist_b(float *b) {
    fastmg->get_coarsist_b(b);
}

extern "C" DLLEXPORT void fastmg_set_coarsist_x(float const *x) {
    fastmg->set_coarsist_x(x);
}

extern "C" DLLEXPORT void fastmg_get_finest_x(float *x) {
    fastmg->get_finest_x(x);
}

extern "C" DLLEXPORT void fastmg_coarse_solve() {
    fastmg->coarse_solve();
}


//extern "C" DLLEXPORT void fastmg_test() {
    //if (!fastmg)
        //fastmg = new VCycle{};
    //fastmg->setup(3);
//
    //CSR<float> &mat = fastmg->levels.at(0).A;
    //auto dat = _load<float>("/tmp/vcycle-new-1.txt");
    //auto ind = _load<int>("/tmp/vcycle-new-2.txt");
    //auto off = _load<int>("/tmp/vcycle-new-3.txt");
    //mat.assign(dat.data(), std::size(dat), ind.data(),
               //std::size(ind), off.data(), std::size(off),
               //off.size() - 1, *std::max_element(ind.begin(), ind.end()) + 1, std::size(dat));
    //Vec<float> &out = fastmg->levels.at(0).outh;
    //Vec<float> &x = fastmg->levels.at(0).h;
    //out.resize(mat.nrows);
    //auto xd = _load<float>("/tmp/vcycle-new-4.txt");
    //x.assign(xd.data(), std::size(xd));
    //Buffer &buf = fastmg->buf5;
    //fastmg->spmv(out, 1, mat, x, 0, buf);
    //xd.resize(out.size());
    //out.store(xd.data());
    //for (int i = 0; i < xd.size(); ++i) {
        //if (xd[i] != 0)
            //printf("!XD %d %e\n", i, xd[i]);
    //}
//}
