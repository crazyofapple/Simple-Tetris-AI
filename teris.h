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
	char pos[4][4]; //��¼�������״
	char minR, maxR; // ��״����С����������
	char minC, maxC; // ��״����С����������
	char maxRow[4], minRow[4];
};
class Teris {
public:
	Teris() = default;
	~Teris();
	Teris(RECT);
	Teris(Teris& rhs);
	void InitGame(); // ��Ϸ���¿�ʼ
	char** GetpDC() { return pDC; }
	uint32_t GetScore() { return score; }
	bool GetGameState() { return fOver; }
	int getNowPtX() { return nowPt.x; }
	int getNowPtY() { return nowPt.y; }
	int getNowShape() { return nowShape; }
	
	bool Up();//��ת����
	bool Down();//��������
	bool Left();//��������
	bool Right();//��������
	void Check();//�жϷ����Ƿ����� ��ʵ��
	void ResetTeris(Teris &teris); // ����

	AI_OFFSET AI(Teris &teris);

private:
	void Init();//��Shape����ĳ�ʼ��
	bool isTDLegal(POINT pt); //�жϴ�λ���ڴ�ֱ�������Ƿ�Ϸ�
	bool isLRLegal(POINT pt); //�жϴ�λ����ˮƽ�������Ƿ�Ϸ�
	void Configure(); //�ı���������
	void Change();//����ͣ��ʱ���Ľ�������״, �ı����������
	bool isClearLine(int line);//�ж��Ƿ�����
	void CopyLine(int srLine, int descLine);
	void initminRow();//���к�����minRow�����ֵ
	void ComputeScore(int clearLines) { score += (uint32_t)pow(2.0, clearLines); }
	void SetNextShape();//��ȡ�¸���״
public:
	
	char **pDC;//��Ϸ��������
	POINT nowPt;//��¼��ǰ����λ��
	Shape *ptr;//ָ��ǰ����
	Shape *nextPtr;//ָ��һ�·���
private:
	Shape sT[4], sL[4], sZ[4], sI[4], sO[4], sRL[4], sRZ[4];
	RECT rect;//��¼��Ϸ�����С����ʵ����Ļ��ʾ��ССʮ����
	char nowShape;//��ǰ������״
	char nextShape; //��һ��������״
	short* minRow; //��¼��Ϸ����ÿһ����ߵ�
	int clearLineCount; //�ϴ���������
	bool fState;//�����Ƿ񴥵�
	int shapeCount;//��¼���·������
	uint32_t score;//�������÷���
	bool fOver;//��Ϸ�Ƿ����
	int maxLine;//��¼�������
	int nRows, nCols;
};


bool CompareAIScore(const pair<AI_OFFSET, int>& l, const pair<AI_OFFSET, int> &r);