#pragma once
#include <Windows.h>
#include <time.h>
#include <math.h>
#include <utility>
using namespace std;
typedef unsigned int	uint32_t;
enum FORM {
	T, L, Z, I, O, RL, RZ
};
struct AI_OFFSET
{
	int offsetX;
	int offsetY;
	int shape;
};
class Shape {
public:
	void InitShape(char pPos[][4], char pMinR, char pMaxR, char pMinC, char pMaxC, char pMaxRow[], char pMinRow[])
	{
		minR = pMinR;
		maxR = pMaxR;
		minC = pMinC;
		maxC = pMaxC;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				pos[i][j] = pPos[i][j];
			}
			maxRow[i] = pMaxRow[i];
			minRow[i] = pMinRow[i];
		}
	}
public:
	char pos[4][4]; //记录方块的形状
	char minR, maxR; // 形状中最小、最大的行数
	char minC, maxC; // 形状中最小、最大的列数
	char maxRow[4], minRow[4];
};
class Teris {
public:
	Teris() = default;
	~Teris();
	Teris(RECT);
	Teris(Teris& rhs);
	void InitGame(); // 游戏重新开始
	char** GetpDC() { return pDC; }
	uint32_t GetScore() { return score; }
	bool GetGameState() { return fOver; }
	int getNowPtX() { return nowPt.x; }
	int getNowPtY() { return nowPt.y; }
	int getNowShape() { return nowShape; }
	
	bool Up();//翻转方块
	bool Down();//方块向下
	bool Left();//方块向左
	bool Right();//方块向右
	void Check();//判断方块是否依靠 待实现
	void ResetTeris(Teris &teris); // 重置

	AI_OFFSET AI(Teris &teris);

private:
	void Init();//对Shape对象的初始化
	bool isTDLegal(POINT pt); //判断此位置在垂直方向上是否合法
	bool isLRLegal(POINT pt); //判断此位置在水平方向上是否合法
	void Configure(); //改变区域数组
	void Change();//方块停靠时，改进方块形状, 改变区域数组等
	bool isClearLine(int line);//判断是否消行
	void CopyLine(int srLine, int descLine);
	void initminRow();//消行后重置minRow数组的值
	void ComputeScore(int clearLines) { score += (uint32_t)pow(2.0, clearLines); }
	void SetNextShape();//获取下个形状
public:
	
	char **pDC;//游戏区域数组
	POINT nowPt;//记录当前方块位置
	Shape *ptr;//指向当前方块
	Shape *nextPtr;//指向一下方块
private:
	Shape sT[4], sL[4], sZ[4], sI[4], sO[4], sRL[4], sRZ[4];
	RECT rect;//记录游戏区域大小（比实际屏幕显示大小小十倍）
	char nowShape;//当前方块形状
	char nextShape; //下一个方块形状
	short* minRow; //记录游戏区域每一列最高点
	int clearLineCount; //上次消掉行数
	bool fState;//方块是否触底
	int shapeCount;//记录掉下方块个数
	uint32_t score;//保存所得分数
	bool fOver;//游戏是否结束
	int maxLine;//记录最高行数
	int nRows, nCols;
};


bool CompareAIScore(const pair<AI_OFFSET, int>& l, const pair<AI_OFFSET, int> &r);