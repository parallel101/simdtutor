#include <iostream>
#include <immintrin.h>
#include <atomic>
#include <vector>
#include <random>
#include <fstream>
#include <stdio.h>
#include <chrono>
#define REP4(x) (x, x, x, x)

struct templateFeat
{
	__m128i x;
	__m128i y;
	__m64 dx;
	__m64 dy;
	__m128 mag;
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
    float anMinScore     = 0.4 - 1;
    float NormMinScore   = 0.4 / template_feat_size;
    float NormGreediness = ((1 - 0.8 * 0.4) / (1 - 0.8)) / template_feat_size;
    
    /* #pragma omp parallel for collapse(2) */
    for(int i = 0; i < 1920; i++)
    {
        for(int j = 0; j < 1200; j++)
        {
            float PartialScore = 0;
            int   SumOfCoords  = 0;
            __m128 $PartialSum = _mm_setzero_ps();

            auto $i = _mm_set1_epi32(i);
            auto $j = _mm_set1_epi32(j);
            auto $0 = _mm_setzero_si128();
            auto $230 = _mm_set1_epi32(230);
            auto $350 = _mm_set1_epi32(350);

            for(int m = 0; m < template_feat_size / 4 * 4; m += 4)
            {
                auto $curX = _mm_add_epi32($i, template_point[m / 4].x);
                auto $curY = _mm_add_epi32($j, template_point[m / 4].y);

                auto $notok = _mm_or_si128(
                    _mm_or_si128(_mm_cmplt_epi32($curX, $0), _mm_cmplt_epi32($curY, $0)),
                    _mm_or_si128(_mm_cmpgt_epi32($curX, $230), _mm_cmpgt_epi32($curY, $350)));
                if (0xffff == _mm_movemask_epi8($notok)) {
                    continue;
                }

                auto $iTx = _mm_cvtepi16_epi32(_mm_set1_epi64(template_point[m / 4].dx));
                auto $iTy = _mm_cvtepi16_epi32(_mm_set1_epi64(template_point[m / 4].dy));
                auto $iTm = template_point[m / 4].mag;

                auto $offSet = _mm_andnot_si128($notok, _mm_add_epi32(_mm_mullo_epi32($curY, $350), $curX));
                auto $iSxy = _mm_i32gather_epi32((int *)search_point.data(), $offSet, sizeof(searchFeat));
                // isx isy isx isy isx isy isx isy
                auto $iSx = _mm_srai_epi32(_mm_slli_epi32($iSxy, 16), 16);
                auto $iSy = _mm_srai_epi32($iSxy, 16);
                auto $iSm = _mm_i32gather_ps(1 + (float *)search_point.data(), $offSet, sizeof(searchFeat));

                auto $accum = _mm_andnot_ps(_mm_castsi128_ps($notok),
                                            _mm_mul_ps(
                                            _mm_mul_ps($iSm, $iTm),
                                            _mm_cvtepi32_ps(
                                            _mm_add_epi32(
                                            _mm_mullo_epi32($iSx, $iTx),
                                            _mm_mullo_epi32($iSy, $iTy)))));
                _mm_set1_ps(PartialSum) + $accum;
                /* auto $cond = _mm_mul_ps($m1, */
                /*                         _mm_min_ps( */
                /*                         _mm_add_ps($anMinScore, _mm_mul_ps($NormGreediness, $m1)), */
                /*                         _mm_mul_ps($NormMinScore, $m1))); */
                /* for (int _ = 0; _ < 4; _++) */
                /*      printf("%d %f %f %f %f %f %f %f %f %f %f %f\n", m + _, $accum[_], _mm_cvtepi32_ps($offSet)[_], _mm_cvtepi32_ps($curX)[_], _mm_cvtepi32_ps($curY)[_], _mm_cvtepi32_ps($iSx)[_], _mm_cvtepi32_ps($iSy)[_], _mm_cvtepi32_ps($iTx)[_], _mm_cvtepi32_ps($iTy)[_], $iSm[_], $iTm[_], _mm_cvtepi32_ps($notok)[_]); */
                /* exit(1); */

                $accum = _mm_add_ps($accum, _mm_castsi128_ps(_mm_bslli_si128(_mm_castps_si128($accum), 4)));
                $accum = _mm_add_ps($accum, _mm_castsi128_ps(_mm_bslli_si128(_mm_castps_si128($accum), 8)));

                bool breaks = false;
                for (int mm = 0; mm < 4; mm++) {
                    auto tmpPartialSum = PartialSum + $accum[mm];
                    SumOfCoords  = m + mm + 1;
                    PartialScore = tmpPartialSum / SumOfCoords;

                    /* printf("%d %d %d %d %d %d %d %d %f %f %f\n", m, offSet, curX, curY, iSx, iSy, iTx, iTy, iSm, iTm, PartialSum); */
                    /* exit(1); */
                    /*  */
                    /* printf("%d %f %f\n", m, PartialSum, ((iSx * iTx) + (iSy * iTy)) * (iSm * iTm)); */

                    if(PartialScore < (std::min(anMinScore + NormGreediness * SumOfCoords, NormMinScore * SumOfCoords)))
                    {
                        breaks = true;
                        break;
                    }
                }
                PartialSum += $accum[3];
                if (breaks) break;
            }
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
    std::mt19937 gen(0), gen1(1), gen2(2), gen3(3), gen4(4), gen5(5);
    std::uniform_int_distribution<int> intXDist(-18, 32);
    std::uniform_int_distribution<int> intYDist(-48, 49);
    std::uniform_int_distribution<short> shortDXDist(-348, 349);
    std::uniform_int_distribution<short> shortDYDist(-421, 352);
    std::uniform_real_distribution<float> floatDist(0.00237112f, 0.0120056f);

    std::vector<templateFeat> template_point(216 / 4);
    for (auto &feat : template_point)
    {
        feat.x = _mm_set_epi32 REP4(intXDist(gen1));
        feat.y = _mm_set_epi32 REP4(intYDist(gen2));
        feat.dx = _mm_set_pi16 REP4(shortDXDist(gen3));
        feat.dy = _mm_set_pi16 REP4(shortDYDist(gen4));
        feat.mag = _mm_set_ps REP4(floatDist(gen5));
    }

    std::uniform_int_distribution<short> shortDxDist(-460, 460);
    std::uniform_int_distribution<short> shortDyDist(-476, 460);
    std::uniform_real_distribution<float> float_Dist(0.0f, 0.707107f);

    std::vector<searchFeat> search_point(2304000);
    for (auto &feat : search_point)
    {
        feat.dx = (shortDxDist(gen));
        /* static int once = printf("%d\n", feat.dx); */
        feat.dy = (shortDyDist(gen));
        feat.mag = (float_Dist(gen));
    }

    //运行算法,计算耗时
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<matchResult> const &results0Deg = calSimilarity(template_point,216,search_point,2304000);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Function took " << duration.count() << " milliseconds." << std::endl;

    //保存匹配结果
    saveDataToTxt("/tmp/match_result.txt",results0Deg);

    return 0;
}
