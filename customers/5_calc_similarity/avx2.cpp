//********************************************************************************************
//***********************************normal_calSimilarity.cpp********************************
//********************************************************************************************
#include <iostream>
#include <immintrin.h>
#include <atomic>
#include <vector>
#include <random>
#include <fstream>
#include <stdio.h>
#include <chrono>
#define REP8(x) (x, x, x, x, x, x, x, x)

struct templateFeat
{
	__m256i x;
	__m256i y;
	__m128i dx;
	__m128i dy;
	__m256 mag;
};

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
        std::cout << "Match result datas saved to " << txt_path << "!" << std::endl;
    } else {
        std::cout << "Unable to open file" << std::endl;
    }
    return true;
}

std::vector<matchResult> const &calSimilarity(const std::vector<templateFeat> &template_point,size_t template_feat_size,const std::vector<searchFeat> &search_point,int search_feat_size)
{
    static std::vector<matchResult> results0Deg;
    std::atomic<size_t> results0DegSize{0};
    results0Deg.resize(1920 * 1200);
    auto $anMinScore     = _mm256_set1_ps(0.4f - 1);
    auto $NormMinScore   = _mm256_set1_ps(0.4f / template_feat_size);
    auto $NormGreediness = _mm256_set1_ps(((1 - 0.8f * 0.4f) / (1 - 0.8f)) / template_feat_size);
    
    #pragma omp parallel for collapse(2)
    for(int i = 0; i < 1920; i++)
    {
        for(int j = 0; j < 1200; j++)
        {
            auto $PartialSum = _mm256_setzero_ps();

            auto $i = _mm256_set1_epi32(i);
            auto $j = _mm256_set1_epi32(j);
            auto $m1 = _mm256_setr_ps(1, 2, 3, 4, 5, 6, 7, 8);
            auto $0 = _mm256_setzero_si256();
            auto $8 = _mm256_set1_ps(4);
            auto $230 = _mm256_set1_epi32(230);
            auto $350 = _mm256_set1_epi32(350);

            for(int m = 0; m < template_feat_size / 8 * 8; m += 8)
            {
                auto $curX = _mm256_add_epi32($i, template_point[m / 8].x);
                auto $curY = _mm256_add_epi32($j, template_point[m / 8].y);

                auto $ok = _mm256_or_si256(
                    _mm256_or_si256(_mm256_srai_epi32($curX, 31), _mm256_srai_epi32($curY, 31)),
                    _mm256_or_si256(_mm256_cmpgt_epi32($curX, $230), _mm256_cmpgt_epi32($curY, $350)));

                auto $iTx = _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(template_point[m / 8].dx));
                auto $iTy = _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(template_point[m / 8].dy));
                auto $iTm = template_point[m / 8].mag;

                auto $offSet = _mm256_and_si256($curY * $350 + $curX, $ok);
                auto $iSxy = _mm256_i32gather_epi32((int *)search_point.data(), $offSet, sizeof(searchFeat));
                // isx isy isx isy isx isy isx isy
                // isx 0 isx 0 isx 0 isx 0
                // isy 0 isy 0 isy 0 isy 0
                auto $iSx = _mm256_cvtepi32_ps(_mm256_srai_epi32(_mm256_unpacklo_epi16($0, $iSxy), 16));
                auto $iSy = _mm256_cvtepi32_ps(_mm256_srai_epi32(_mm256_unpackhi_epi16($0, $iSxy), 16));
                auto $iSm = _mm256_i32gather_ps(1 + (float *)search_point.data(), $offSet, sizeof(searchFeat));

                $PartialSum = _mm256_add_ps($PartialSum,
                                         _mm256_and_ps(_mm256_castsi256_ps($ok),
                                                    _mm256_mul_ps(
                                                    _mm256_mul_ps($iSm, $iTm),
                                                    _mm256_add_ps(
                                                    _mm256_mul_ps($iSx, $iTx),
                                                    _mm256_mul_ps($iSy, $iTy))
                                                    )));
                auto $cond = _mm256_mul_ps($m1,
                                        _mm256_min_ps(
                                        _mm256_add_ps($anMinScore, _mm256_mul_ps($NormGreediness, $m1)),
                                        _mm256_mul_ps($NormMinScore, $m1)));
                /* for (int _ = 0; _ < 8; _++) */
                /*      printf("%f %f %f %f %f %f %f %f\n", $iSx[_], $iSy[_], $iTx[_], $iTy[_], $iSm[_], $PartialSum[_], $m1[_], $cond[_]); */

                auto $cmp = _mm256_cmp_ps($PartialSum, $cond, _CMP_LT_OQ);
                int cmpmask = _mm256_movemask_ps($cmp);
                if (cmpmask) [[unlikely]] {
                    $PartialSum = _mm256_setzero_ps();
                    break;
                }
                $m1 = _mm256_add_ps($m1, $8);
            }
            $PartialSum = _mm256_hadd_ps($PartialSum, $PartialSum);
            $PartialSum = _mm256_hadd_ps($PartialSum, $PartialSum);
            $PartialSum = _mm256_hadd_ps($PartialSum, $PartialSum);
            auto PartialScore = _mm256_cvtss_f32($PartialSum) / (template_feat_size + 1);
            if(PartialScore > 0.4f)
            {
                results0Deg[results0DegSize.fetch_add(1, std::memory_order_relaxed)] = {i, j, 0, PartialScore};
            }
        }
    }

    results0Deg.resize(results0DegSize.load(std::memory_order_relaxed));
    return results0Deg;
}

int main()
{
    //为了测试函数而生成的随机数：template_point/search_point
    std::mt19937 gen;
    std::uniform_int_distribution<int> intXDist(-18, 32);
    std::uniform_int_distribution<int> intYDist(-48, 49);
    std::uniform_int_distribution<short> shortDXDist(-348, 349);
    std::uniform_int_distribution<short> shortDYDist(-421, 352);
    std::uniform_real_distribution<float> floatDist(0.00237112f, 0.0120056f);

    std::vector<templateFeat> template_point(215 / 8);
    for (auto &feat : template_point)
    {
        feat.x = _mm256_setr_epi32 REP8(intXDist(gen));
        feat.y = _mm256_setr_epi32 REP8(intYDist(gen));
        feat.dx = _mm_setr_epi16 REP8(shortDXDist(gen));
        feat.dy = _mm_setr_epi16 REP8(shortDYDist(gen));
        feat.mag = _mm256_setr_ps REP8(floatDist(gen));
    }

    std::uniform_int_distribution<short> shortDxDist(-460, 460);
    std::uniform_int_distribution<short> shortDyDist(-476, 460);
    std::uniform_real_distribution<float> float_Dist(0.0f, 0.707107f);

    std::vector<searchFeat> search_point(2304000);
    for (auto &feat : search_point)
    {
        feat.dx = (shortDxDist(gen));
        feat.dy = (shortDyDist(gen));
        feat.mag = (float_Dist(gen));
    }

    //运行算法,计算耗时
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<matchResult> const &results0Deg = calSimilarity(template_point,215,search_point,2304000);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Function took " << duration.count() << " milliseconds." << std::endl;

    //保存匹配结果
    saveDataToTxt("/tmp/match_result.txt",results0Deg);

    return 0;
}
