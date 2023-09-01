//********************************************************************************************
//***********************************normal_calSimilarity.cpp********************************
//********************************************************************************************
//#define USE_MULTISCORE//causing wrong result
//#define USE_FIRSTUNROLL
#define USE_OPENMP
#define USE_UNROLLM4
#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <stdio.h>
#include <chrono>
#include <immintrin.h>
#ifdef USE_OPENMP
#include <atomic>
#endif

struct templateFeat
{
	int x;
	int y;
	short dx;
	short dy;
	float mag;
};

#ifdef USE_MULTISCORE
struct templateFeatSimd
{
	__m256i x;
	__m256i y;
	__m128i dx;
	__m128i dy;
	__m256 mag;
};
#endif

struct matchResult
{
	int i;
	int j;
	int angle;
	float score;
};

struct searchFeat
{
	short dx;
	short dy;
	float mag;
};

bool saveDataToTxt(const std::string txt_path,const std::vector<matchResult> match_result) {
    std::ofstream file(txt_path);

    if (file.is_open()) {
        for (int i = 0; i < match_result.size(); ++i) {
            file << match_result[i].i << " " << match_result[i].j << " " << match_result[i].angle << " " << match_result[i].score << std::endl;
        }

        file.close();
        std::cout << "Match result datas saved to match_result.txt!" << std::endl;
    } else {
        std::cout << "Unable to open file" << std::endl;
    }
    return true;
}

#ifdef USE_MULTISCORE
static float multiScore(templateFeatSimd const *template_point_simd,
                        searchFeat const *search_point,
                        float const *thresholds,
                        int template_feat_size,
                        int search_feat_size,
                        int i, int j, int mbase,
                        float PartialSumBase) {
    auto $PartialScore = _mm256_setzero_ps();
    auto $PartialSum = _mm256_set1_ps(PartialSumBase);
    auto $SumOfCoords = _mm256_add_ps(_mm256_setr_ps(1, 2, 3, 4, 5, 6, 7, 8), _mm256_set1_ps(mbase));
    auto $8 = _mm256_set1_ps(8);
    auto $template_feat_size_plus1 = _mm256_set1_ps(template_feat_size + 1);
    auto $i = _mm256_set1_epi32(i);
    auto $j = _mm256_set1_epi32(j);
    auto $0 = _mm256_setzero_si256();
    auto $230 = _mm256_set1_epi32(230);
    auto $350 = _mm256_set1_epi32(350);

    /* printf("%d\n", mbase); */
    for(int m = mbase; m < ((template_feat_size + 7) & ~7); m += 8, $SumOfCoords = _mm256_add_ps($SumOfCoords, $8))
    {
        auto $curX = _mm256_add_epi32($i, template_point_simd[m / 8].x);
        auto $curY = _mm256_add_epi32($j, template_point_simd[m / 8].y);

        auto $notok = _mm256_or_si256(_mm256_or_si256(_mm256_castps_si256(_mm256_cmp_ps($SumOfCoords, $template_feat_size_plus1, _CMP_GE_OQ)),
                                                _mm256_or_si256(_mm256_srai_epi32($curX, 31), _mm256_srai_epi32($curY, 31))),
                                   _mm256_or_si256(_mm256_cmpgt_epi32($curX, $230), _mm256_cmpgt_epi32($curY, $350)));
        /* if (0xffff == _mm256_movemask_epi8($notok)) { */
        /*     continue; */
        /* } */

        auto $iTx = _mm256_cvtepi16_epi32(template_point_simd[m / 8].dx);
        auto $iTy = _mm256_cvtepi16_epi32(template_point_simd[m / 8].dy);
        auto $iTm = template_point_simd[m / 8].mag;

        auto $offSet = _mm256_andnot_si256($notok, _mm256_add_epi32(_mm256_mullo_epi32($curY, $350), $curX));
        auto $iSxy = _mm256_i32gather_epi32((int *)search_point, $offSet, sizeof(searchFeat));
        // isx isy isx isy isx isy isx isy
        auto $iSx = _mm256_srai_epi32(_mm256_slli_epi32($iSxy, 16), 16);
        auto $iSy = _mm256_srai_epi32($iSxy, 16);
        auto $iSm = _mm256_i32gather_ps(1 + (float *)search_point, $offSet, sizeof(searchFeat));

        auto $accum = _mm256_andnot_ps(_mm256_castsi256_ps($notok),
                                    _mm256_mul_ps(
                                    _mm256_mul_ps($iSm, $iTm),
                                    _mm256_cvtepi32_ps(
                                    _mm256_add_epi32(
                                    _mm256_mullo_epi32($iSx, $iTx),
                                    _mm256_mullo_epi32($iSy, $iTy)))));
        /* auto $cond = _mm256_mul_ps($m1, */
        /*                         _mm256_min_ps( */
        /*                         _mm256_add_ps($anMinScore, _mm256_mul_ps($NormGreediness, $m1)), */
        /*                         _mm256_mul_ps($NormMinScore, $m1))); */
        /* for (int _ = 0; _ < 4; _++) */
        /*      printf("%d %f %f %f %f %f %f %f %f %f %f %f\n", m + _, $accum[_], _mm256_cvtepi32_ps($offSet)[_], _mm256_cvtepi32_ps($curX)[_], _mm256_cvtepi32_ps($curY)[_], _mm256_cvtepi32_ps($iSx)[_], _mm256_cvtepi32_ps($iSy)[_], _mm256_cvtepi32_ps($iTx)[_], _mm256_cvtepi32_ps($iTy)[_], $iSm[_], $iTm[_], _mm256_cvtepi32_ps($notok)[_]); */
        /* exit(1); */

        /* for (int _ = 0; _ < 8; _++) */
        /*      printf("%d %f\n", _, $accum[_]); */

        // 0 1 2 3 4 5 6 7
        $accum = _mm256_add_ps($accum, _mm256_castsi256_ps(_mm256_slli_si256(_mm256_castps_si256($accum), 4)));
        // 0 1+0 2+1 3+2 4 5+4 6+5 7+6
        $accum = _mm256_add_ps($accum, _mm256_castsi256_ps(_mm256_slli_si256(_mm256_castps_si256($accum), 8)));
        // 0 1+0 2+1+0 3+2+1+0 4 5+4 6+5+4 7+6+5+4
        $accum = _mm256_add_ps($accum, _mm256_set_m128(_mm_permute_ps(_mm256_castps256_ps128($accum), 0xff), _mm_setzero_ps()));
        // 0 1+0 2+1+0 3+2+1+0 3+2+1+0+4 3+2+1+0+5+4 3+2+1+0+6+5+4 3+2+1+0+7+6+5+4

        /* for (int _ = 0; _ < 8; _++) */
        /*      printf("%d %f\n", _, $accum[_]); */
        /* exit(1); */

        $PartialScore = _mm256_add_ps($PartialSum, $accum);
        /* auto $cond = _mm256_mul_ps($SumOfCoords, _mm256_min_ps( */
        /*     _mm256_add_ps(_mm256_set1_ps(anMinScore), _mm256_mul_ps(_mm256_set1_ps(NormGreediness), $SumOfCoords)), */
        /*     _mm256_mul_ps(_mm256_set1_ps(NormMinScore), $SumOfCoords))); */
        auto $cond = _mm256_load_ps(thresholds + m);
        auto $cmp = _mm256_andnot_ps(_mm256_castsi256_ps($notok), _mm256_cmp_ps($PartialScore, $cond, _CMP_LT_OQ));
        int cmpmask = _mm256_movemask_ps($cmp);
        if (cmpmask) [[unlikely]] {
            int bit = __builtin_ctz(cmpmask);
            return $PartialScore[bit] / (m + bit + 1);
        }
        $PartialSum = _mm256_add_ps($PartialSum, _mm256_broadcastss_ps(_mm256_castps256_ps128(_mm256_castpd_ps(_mm256_permute4x64_pd(_mm256_castps_pd(_mm256_permute_ps($accum, 0xff)), 0xf)))));
    }

        /* for (int _ = 0; _ < 8; _++) */
        /*      printf("%d %f %f\n", _, $PartialSum[_], $SumOfCoords[_]); */
        /* exit(1); */

    $PartialScore = _mm256_div_ps($PartialScore, $SumOfCoords);
    return $PartialScore[(template_feat_size - 1) & 7];
}
#endif

std::vector<matchResult> const &calSimilarity(const std::vector<templateFeat> &template_point,const std::vector<searchFeat> &search_point)
{
    int template_feat_size =  template_point.size();
    int search_feat_size =  search_point.size();

#ifdef USE_MULTISCORE
    static std::vector<templateFeatSimd> template_point_simd;
    template_point_simd.resize((template_feat_size + 7) & ~7);
    for (int m = 0; m < template_point_simd.size() * 8; m += 8) {
        template_point_simd[m / 8].x = _mm256_setr_epi32(template_point[(m) % template_feat_size].x, template_point[(m + 1) % template_feat_size].x, template_point[(m + 2) % template_feat_size].x, template_point[(m + 3) % template_feat_size].x, template_point[(m + 4) % template_feat_size].x, template_point[(m + 4 + 1) % template_feat_size].x, template_point[(m + 4 + 2) % template_feat_size].x, template_point[(m + 4 + 3) % template_feat_size].x);
        template_point_simd[m / 8].y = _mm256_setr_epi32(template_point[(m) % template_feat_size].y, template_point[(m + 1) % template_feat_size].y, template_point[(m + 2) % template_feat_size].y, template_point[(m + 3) % template_feat_size].y, template_point[(m + 4) % template_feat_size].y, template_point[(m + 4 + 1) % template_feat_size].y, template_point[(m + 4 + 2) % template_feat_size].y, template_point[(m + 4 + 3) % template_feat_size].y);
        template_point_simd[m / 8].dx = _mm_setr_epi16(template_point[(m) % template_feat_size].dx, template_point[(m + 1) % template_feat_size].dx, template_point[(m + 2) % template_feat_size].dx, template_point[(m + 3) % template_feat_size].dx, template_point[(m + 4) % template_feat_size].dx, template_point[(m + 4 + 1) % template_feat_size].dx, template_point[(m + 4 + 2) % template_feat_size].dx, template_point[(m + 4 + 3) % template_feat_size].dx);
        template_point_simd[m / 8].dy = _mm_setr_epi16(template_point[(m) % template_feat_size].dy, template_point[(m + 1) % template_feat_size].dy, template_point[(m + 2) % template_feat_size].dy, template_point[(m + 3) % template_feat_size].dy, template_point[(m + 4) % template_feat_size].dy, template_point[(m + 4 + 1) % template_feat_size].dy, template_point[(m + 4 + 2) % template_feat_size].dy, template_point[(m + 4 + 3) % template_feat_size].dy);
        template_point_simd[m / 8].mag = _mm256_setr_ps(template_point[(m) % template_feat_size].mag, template_point[(m + 1) % template_feat_size].mag, template_point[(m + 2) % template_feat_size].mag, template_point[(m + 3) % template_feat_size].mag, template_point[(m + 4) % template_feat_size].mag, template_point[(m + 4 + 1) % template_feat_size].mag, template_point[(m + 4 + 2) % template_feat_size].mag, template_point[(m + 4 + 3) % template_feat_size].mag);
    }
#endif

    static std::vector<matchResult> results0Deg;
    results0Deg.resize(1920 * 1200);
#ifdef USE_OPENMP
    std::atomic<size_t> results0DegSize = 0;
#else
    size_t results0DegSize = 0;
#endif
    float anMinScore     = 0.4 - 1;
    float NormMinScore   = 0.4 / template_feat_size;
    float NormGreediness = ((1 - 0.8 * 0.4) / (1 - 0.8)) / template_feat_size;

    static std::vector<float> thresholds;
    thresholds.resize(((template_feat_size + 7) & ~7));
    for (int m = 0; m < ((template_feat_size + 7) & ~7); m++) {
        auto SumOfCoords = m + 1;
        thresholds[m] = SumOfCoords * std::min(anMinScore + NormGreediness * SumOfCoords, NormMinScore * SumOfCoords);
    }
    
#ifdef USE_OPENMP
    #pragma omp parallel for collapse(2)
#endif
    for(int i = 0; i < 1920; i++)
    {
        for(int j = 0; j < 1200; j += 8)
        {
            int m;
            auto $PartialSum   = _mm256_setzero_ps();
            #ifdef USE_FIRSTUNROLL
            auto $SumOfCoords  = _mm256_set1_epi32(1);
            #else
            auto $SumOfCoords  = _mm256_set1_epi32(1);
            #endif
            auto $j = _mm256_add_epi32(_mm256_set1_epi32(j), _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7));
            auto $350 = _mm256_set1_epi32(350);
            auto $loopterm = _mm256_setzero_si256();

            #ifdef USE_FIRSTUNROLL
            {
                int curX = i + template_point[0].x;
                if (!(curX < 0 || curX > 230)) [[likely]] {
                    auto $curY = _mm256_add_epi32($j, _mm256_set1_epi32(template_point[0].y));
                    auto $notok = _mm256_or_si256(_mm256_srai_epi32($curY, 31), _mm256_cmpgt_epi32($curY, $350));

                    int iTx = template_point[0].dx;
                    int iTy = template_point[0].dy;
                    float iTm = template_point[0].mag;

                    /* int offSet = curY * 350 + curX; */
                    auto $offSet = _mm256_andnot_si256($notok, _mm256_add_epi32(_mm256_mullo_epi32($curY, $350), _mm256_set1_epi32(curX)));
                    auto $iSxy = _mm256_i32gather_epi32((int *)search_point.data(), $offSet, sizeof(searchFeat));
                    // isx isy isx isy isx isy isx isy
                    auto $iSx = _mm256_srai_epi32(_mm256_slli_epi32($iSxy, 16), 16);
                    auto $iSy = _mm256_srai_epi32($iSxy, 16);
                    auto $iSm = _mm256_i32gather_ps(1 + (float *)search_point.data(), $offSet, sizeof(searchFeat));

                    auto $accum = _mm256_andnot_ps(_mm256_castsi256_ps($notok),
                                                _mm256_mul_ps(
                                                _mm256_mul_ps($iSm, _mm256_set1_ps(iTm)),
                                                _mm256_cvtepi32_ps(
                                                _mm256_add_epi32(
                                                _mm256_mullo_epi32($iSx, _mm256_set1_epi32(iTx)),
                                                _mm256_mullo_epi32($iSy, _mm256_set1_epi32(iTy))))));
                    $PartialSum = _mm256_add_ps($PartialSum, $accum);

                    auto $cmp = _mm256_andnot_ps(_mm256_castsi256_ps($notok), _mm256_cmp_ps($PartialSum, _mm256_set1_ps(thresholds[0]), _CMP_LT_OQ));
                    $loopterm = _mm256_castps_si256($cmp);
                    $SumOfCoords = _mm256_add_epi32($SumOfCoords, _mm256_andnot_si256($loopterm, _mm256_set1_epi32(1)));
                }
            }
            if (_mm256_movemask_epi8($loopterm) != -1) {
                int curX = i + template_point[1].x;
                if (!(curX < 0 || curX > 230)) [[likely]] {
                    auto $curY = _mm256_add_epi32($j, _mm256_set1_epi32(template_point[1].y));
                    auto $notok = _mm256_or_si256($loopterm, _mm256_or_si256(_mm256_srai_epi32($curY, 31), _mm256_cmpgt_epi32($curY, $350)));

                    int iTx = template_point[1].dx;
                    int iTy = template_point[1].dy;
                    float iTm = template_point[1].mag;

                    /* int offSet = curY * 350 + curX; */
                    auto $offSet = _mm256_andnot_si256($notok, _mm256_add_epi32(_mm256_mullo_epi32($curY, $350), _mm256_set1_epi32(curX)));
                    auto $iSxy = _mm256_i32gather_epi32((int *)search_point.data(), $offSet, sizeof(searchFeat));
                    // isx isy isx isy isx isy isx isy
                    auto $iSx = _mm256_srai_epi32(_mm256_slli_epi32($iSxy, 16), 16);
                    auto $iSy = _mm256_srai_epi32($iSxy, 16);
                    auto $iSm = _mm256_i32gather_ps(1 + (float *)search_point.data(), $offSet, sizeof(searchFeat));

                    auto $accum = _mm256_andnot_ps(_mm256_castsi256_ps($notok),
                                                _mm256_mul_ps(
                                                _mm256_mul_ps($iSm, _mm256_set1_ps(iTm)),
                                                _mm256_cvtepi32_ps(
                                                _mm256_add_epi32(
                                                _mm256_mullo_epi32($iSx, _mm256_set1_epi32(iTx)),
                                                _mm256_mullo_epi32($iSy, _mm256_set1_epi32(iTy))))));
                    $PartialSum = _mm256_add_ps($PartialSum, $accum);

                    auto $cmp = _mm256_andnot_ps(_mm256_castsi256_ps($notok), _mm256_cmp_ps($PartialSum, _mm256_set1_ps(thresholds[1]), _CMP_LT_OQ));
                    $loopterm = _mm256_or_si256($loopterm, _mm256_castps_si256($cmp));
                    $SumOfCoords = _mm256_add_epi32($SumOfCoords, _mm256_andnot_si256($loopterm, _mm256_set1_epi32(1)));
                }
                if (_mm256_movemask_epi8($loopterm) != -1) [[unlikely]] {
                    m = 2;
                    #else
                    m = 0;
                    #endif

                    #ifdef USE_UNROLLM4
                    #pragma GCC unroll 4
                    #endif
                    for(;
                        m < template_feat_size;
                        m++, $SumOfCoords = _mm256_add_epi32($SumOfCoords, _mm256_andnot_si256($loopterm, _mm256_set1_epi32(1)))
                    ) {
                        int curX = i + template_point[m].x;
                        if (curX < 0 || curX > 230) [[unlikely]] { continue; }

                        auto $curY = _mm256_add_epi32($j, _mm256_set1_epi32(template_point[m].y));
                        auto $notok = _mm256_or_si256($loopterm, _mm256_or_si256(_mm256_srai_epi32($curY, 31), _mm256_cmpgt_epi32($curY, $350)));

                        int iTx = template_point[m].dx;
                        int iTy = template_point[m].dy;
                        float iTm = template_point[m].mag;

                        /* int offSet = curY * 350 + curX; */
                        auto $offSet = _mm256_andnot_si256($notok, _mm256_add_epi32(_mm256_mullo_epi32($curY, $350), _mm256_set1_epi32(curX)));
                        auto $iSxy = _mm256_i32gather_epi32((int *)search_point.data(), $offSet, sizeof(searchFeat));
                        // isx isy isx isy isx isy isx isy
                        auto $iSx = _mm256_srai_epi32(_mm256_slli_epi32($iSxy, 16), 16);
                        auto $iSy = _mm256_srai_epi32($iSxy, 16);
                        auto $iSm = _mm256_i32gather_ps(1 + (float *)search_point.data(), $offSet, sizeof(searchFeat));

                        auto $accum = _mm256_andnot_ps(_mm256_castsi256_ps($notok),
                                                    _mm256_mul_ps(
                                                    _mm256_mul_ps($iSm, _mm256_set1_ps(iTm)),
                                                    _mm256_cvtepi32_ps(
                                                    _mm256_add_epi32(
                                                    _mm256_mullo_epi32($iSx, _mm256_set1_epi32(iTx)),
                                                    _mm256_mullo_epi32($iSy, _mm256_set1_epi32(iTy))))));
                        $PartialSum = _mm256_add_ps($PartialSum, $accum);

                        auto $cmp = _mm256_andnot_ps(_mm256_castsi256_ps($notok), _mm256_cmp_ps($PartialSum, _mm256_set1_ps(thresholds[m]), _CMP_LT_OQ));
                        $loopterm = _mm256_or_si256($loopterm, _mm256_castps_si256($cmp));
                        #ifndef USE_MULTISCORE
                        if (_mm256_movemask_epi8($loopterm) == -1) {
                            break;
                        }
                        #else
                        auto popcorn = __builtin_popcount(_mm256_movemask_ps(_mm256_castsi256_ps($loopterm)));
                        if ((popcorn == 8 || popcorn >= 7) && !(m & 7)) [[unlikely]] {
                            break;
                        }
                        #endif
                    }
            #ifdef USE_FIRSTUNROLL
                }
            }
            #endif

            auto $PartialScore = _mm256_div_ps($PartialSum, _mm256_cvtepi32_ps($SumOfCoords));
            #ifdef USE_MULTISCORE
            auto mask = _mm256_movemask_ps(_mm256_castsi256_ps($loopterm));
            if (mask != 0xff && m < template_feat_size) {
                auto k = __builtin_ctz(~mask);
                $PartialScore[k] = multiScore(
                    template_point_simd.data(), search_point.data(), thresholds.data(),
                    template_feat_size, search_feat_size, i, j, m, $PartialSum[k]);
            }
            #endif
            auto $PartialScoreAbove = _mm256_cmp_ps($PartialScore, _mm256_set1_ps(0.4f), _CMP_GT_OQ);
            for (int k = 0; k < 8; k++) {
                if($PartialScoreAbove[k]) {
                /* printf("%d %d %f\n", i, j, PartialScore); */
                /*     exit(1); */
#ifdef USE_OPENMP
                    results0Deg[results0DegSize.fetch_add(1, std::memory_order_relaxed)] = {i, j + k, 0, $PartialScore[k]};
#else
                    results0Deg[results0DegSize++] = {i, j + k, 0, $PartialScore[k]};
#endif
                }
            }
        }
    }

#ifdef USE_OPENMP
    results0Deg.resize(results0DegSize.load(std::memory_order_relaxed));
#else
    results0Deg.resize(results0DegSize);
#endif
    return results0Deg;
}

int main()
{
    //为了测试函数而生成的随机数：template_point/search_point
    std::mt19937 gen(0), gen1(1), gen2(2), gen3(3), gen4(4), gen5(5);
    std::uniform_int_distribution<int> intXDist(-18, 32);
    std::uniform_int_distribution<int> intYDist(-48, 49);
    std::uniform_int_distribution<short> shortDXDist(-348, 349);
    std::uniform_int_distribution<short> shortDYDist(-421, 352);
    std::uniform_real_distribution<float> floatDist(0.00237112f, 0.0120056f);

    std::vector<templateFeat> template_point(215);
    for (auto &feat : template_point)
    {
        feat.x = intXDist(gen1);
        feat.y = intYDist(gen2);
        feat.dx = shortDXDist(gen3);
        feat.dy = shortDYDist(gen4);
        feat.mag = floatDist(gen5);
    }

    std::uniform_int_distribution<short> shortDxDist(-460, 460);
    std::uniform_int_distribution<short> shortDyDist(-476, 460);
    std::uniform_real_distribution<float> float_Dist(0.0f, 0.707107f);

    std::vector<searchFeat> search_point(2304000);
    for (auto &feat : search_point)
    {
        feat.dx = shortDxDist(gen);
        /* static int once = printf("%d\n", feat.dx); */
        feat.dy = shortDyDist(gen);
        feat.mag = float_Dist(gen);
    }

    //运行算法,计算耗时
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<matchResult> const &results0Deg = calSimilarity(template_point,search_point);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Function took " << duration.count() << " milliseconds." << std::endl;

    //保存匹配结果
    saveDataToTxt("/tmp/match_result.txt",results0Deg);

    return 0;
}
