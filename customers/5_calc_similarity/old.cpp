//********************************************************************************************
//***********************************normal_calSimilarity.cpp********************************
//********************************************************************************************
#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <stdio.h>
#include <chrono>

struct templateFeat
{
	int x;
	int y;
	short dx;
	short dy;
	float mag;
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
        std::cout << "Match result datas saved to match_result.txt!" << std::endl;
    } else {
        std::cout << "Unable to open file" << std::endl;
    }
    return true;
}

std::vector<matchResult> calSimilarity(const std::vector<templateFeat> template_point,const std::vector<searchFeat> search_point)
{
    int template_feat_size =  template_point.size();
    int search_feat_size =  search_point.size();

    std::vector<matchResult> results0Deg;
    float anMinScore     = 0.4 - 1;
    float NormMinScore   = 0.4 / template_feat_size;
    float NormGreediness = ((1 - 0.8 * 0.4) / (1 - 0.8)) / template_feat_size;
    
    for(int i = 0; i < 1920; i++)
    {
        for(int j = 0; j < 1200; j++)
        {
            float PartialScore = 0;
            float PartialSum   = 0;
            int   SumOfCoords  = 0;

            for(int m = 0; m < template_feat_size; m++)
            {
                int curX = i + template_point[m].x;
                int curY = j + template_point[m].y;
				
                if(curX < 0 || curY < 0 || curX > 230 || curY > 350)
                {
                    continue;
                }

                int iTx = template_point[m].dx;
                int iTy = template_point[m].dy;
                float iTm = template_point[m].mag;

                int offSet = curY * 350 + curX;
                int iSx        = search_point[offSet].dx;
                int iSy        = search_point[offSet].dy; 
                float iSm        = search_point[offSet].mag;

                if((iSx != 0 || iSy != 0) && (iTx != 0 || iTy != 0))
                {
                    PartialSum += ((iSx * iTx) + (iSy * iTy)) * (iSm * iTm);
                }
                SumOfCoords  = m + 1;
                PartialScore = PartialSum / SumOfCoords;

                /* printf("%d %f %d %d %d %d %d %d %d %f %f %f\n", m, ((iSx * iTx) + (iSy * iTy)) * (iSm * iTm), offSet, curX, curY, iSx, iSy, iTx, iTy, iSm, iTm, PartialSum); */
                /* exit(1); */
                /*  */
                /* printf("%d %f %f\n", m, PartialSum, ((iSx * iTx) + (iSy * iTy)) * (iSm * iTm)); */

                if(PartialScore < (std::min(anMinScore + NormGreediness * SumOfCoords, NormMinScore * SumOfCoords)))
                {
                    break;
                }
            }

            /* printf("%f %d\n", PartialScore, SumOfCoords); */
            if(PartialScore > 0.4)
            {
            /* printf("%d %d %f\n", i, j, PartialScore); */
            /*     exit(1); */
                results0Deg.push_back({i, j, 0, PartialScore});
            } 
        }
    }

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
    std::vector<matchResult> results0Deg = calSimilarity(template_point,search_point);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Function took " << duration.count() << " milliseconds." << std::endl;

    //保存匹配结果
    saveDataToTxt("./match_result.txt",results0Deg);

    return 0;
}
