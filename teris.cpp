#include "teris.h"
#include <vector>
#include <algorithm>
using namespace std;


//pass
Teris::~Teris()
{
	if (pDC)
	{
		for (int i = 0; i < nRows; i++)
		{
			delete[] pDC[i];
		}
		delete[] pDC;
	}
	if (minRow)
		delete[] minRow;
}

//pass
Teris::Teris(RECT rect_):rect(rect_)
{
	srand((unsigned int)time(NULL));
	pDC = NULL;
	nRows = rect.bottom - rect.top;
	nCols = rect.right - rect.left;
	Init();
	
	pDC = new char*[nRows];
	for (int i = 0; i < nRows; i++) {
		pDC[i] = new char[nCols];
	}
	minRow = new short[nCols];
	InitGame();
}
//pass
Teris::Teris(Teris & rhs)
{
	fOver = false;
	rect = rhs.rect;
	pDC = NULL;
	minRow = NULL;
	nRows = rect.bottom - rect.top;
	nCols = rect.right - rect.left;
	Init();
	pDC = new char*[nRows];
	for (int i = 0; i < nRows; i++) {
		pDC[i] = new char[nCols];
	}
	minRow = new short[nCols];
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			pDC[i][j] = rhs.pDC[i][j];
		}
	}
	for (int i = 0; i < nCols; i++)
	{
		minRow[i] = rhs.minRow[i];
	}
	maxLine = rhs.maxLine;
	nowPt = rhs.nowPt;
	nowShape = rhs.nowShape;
	nextShape = rhs.nextShape;
	ptr = rhs.ptr;
	nextPtr = rhs.nextPtr;
	score = rhs.score;
}
//pass
void Teris::InitGame()
{
	maxLine = nRows;
	fOver = false;
	score = 0;
	fState = false;
	SetNextShape();
	ptr = nextPtr;
	nowShape = nextShape;
	SetNextShape();
	nowPt.x = (nCols / 2) - ptr->maxC;
	nowPt.y = -(ptr->maxR + 1);
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			pDC[i][j] = 0;
		}
	}
	for (int i = 0; i < nCols; i++)
	{
		minRow[i] = (short)nRows;
	}
}
//pass
bool Teris::Up()
{
	if (fOver) return false;
	Shape *temp = ptr;
	ptr = (ptr - nowShape + (nowShape + 1) % 4);
	if (isLRLegal(nowPt)) {
		nowShape = (nowShape + 1) % 4;
		return true;
	}
	ptr = temp;
	return false;
}
//pass
bool Teris::Down()
{
	if (fOver) return false;

	++nowPt.y;
	if (isTDLegal(nowPt)) {
		return true;
	}
	--nowPt.y;
	return false;
}
//pass
bool Teris::Left()
{
	if (fOver) return false;

	--nowPt.x;
	if (isLRLegal(nowPt)) {
		return true;
	}
	++nowPt.x;
	return false;
}
//pass
bool Teris::Right()
{
	if (fOver) return false;

	++nowPt.x;
	if (isLRLegal(nowPt)) {
		return true;
	}
	--nowPt.x;
	return false;
}
//pass
void Teris::Check()
{
	Change();
}
//pass
void Teris::ResetTeris(Teris & teris)
{
	fOver = teris.fOver;
	rect = teris.rect;
	nRows = rect.bottom - rect.top;
	nCols = rect.right - rect.left;
	for (int i = 0; i < nRows; i++) {
		for (int j = 0; j < nCols; j++) {
			pDC[i][j] = teris.pDC[i][j];
		}
	}
	for (int i = 0; i < nCols; i++) {
		minRow[i] = teris.minRow[i];
	}
	maxLine = teris.maxLine;
	nowPt = teris.nowPt;
	nowShape = teris.nowShape;
	nextShape = teris.nextShape;
	ptr = teris.ptr;
	nextPtr = teris.nextPtr;
	score = teris.score;
}
//pass
AI_OFFSET Teris::AI(Teris & teris)
{
	typedef vector<pair<AI_OFFSET, int> > AI_SCORE;

	AI_OFFSET ao;
	AI_SCORE list;
	Teris temp(*this);
	for (int i = 0; i < 4; i++) {
		Shape* tPtr = (ptr - (int)nowShape + i);
		int pos = -tPtr->minC;
		while (pos + tPtr->maxC < nCols) {
			int score = 0;
			temp.ptr = temp.ptr - nowShape + i;
			temp.nowPt.x = pos++;
			int nowX = temp.nowPt.x;
			int offset = temp.rect.bottom;
			for (int i = temp.ptr->minC; i <= temp.ptr->maxC; i++)
			{
				if (temp.minRow[temp.nowPt.x + i] - temp.ptr->maxRow[i] - 1<offset)
				{
					offset = temp.minRow[temp.nowPt.x + i] - temp.ptr->maxRow[i] - 1;
				}
			}
			if (offset< nRows * 3 / 5)
			{
				score -= (nRows * 3 / 5 - offset) * 2;
			}
			int l = nowX + temp.ptr->minC;
			int r = nowX + temp.ptr->maxC;
			temp.nowPt.y = offset;
			temp.Change();
			if (!temp.fOver)
			{
				score += (pow(temp.clearLineCount, 2.0) - (temp.clearLineCount - 1));
				//  score += temp.clearLineCount * 2;
				for (int j = l; j <= r; j++)
				{
					for (int k = temp.minRow[j]; k<nRows; k++)
					{
						if (temp.pDC[k][j] == 0)
						{
							score -= 4;
						}
					}
					for (int k = minRow[j]; k<nRows; k++)
					{
						if (pDC[k][j] == 0)
						{
							score += 4;
						}
					}
				}
				int count = 0;
				for (int j = temp.minRow[1]; j <= nRows; j++)
				{
					if (j == nRows || temp.pDC[j][0] == 1)
					{
						while (count>0)
						{
							score -= count--;
						}
						continue;
					}
					count++;
				}
				for (int j = 0; j<nCols - 2; j++)
				{
					int maxR = temp.minRow[j] >= temp.minRow[j + 2] ?
						temp.minRow[j] : temp.minRow[j + 2];
					for (int k = maxR; k <= nRows; k++)
					{
						if (k == nRows || temp.pDC[k][j + 1] == 1)
						{
							while (count>0)
							{
								score -= count--;
							}
							continue;
						}
						++count;
					}
				}
				for (int j = temp.minRow[nCols - 2];
					j <= nRows; ++j)
				{
					if (j == nRows ||
						temp.pDC[j][nCols - 1] == 1)
					{
						while (count>0)
						{
							score -= count--;
						}
						continue;
					}
					++count;
				}
				for (int j = temp.maxLine; j<nRows; j++)
				{
					int flag = temp.pDC[j][0];
					for (int k = 1; k<nCols; k++)
					{
						if (flag != temp.pDC[j][k])
						{
							score -= 1;
							flag = !flag;
						}
					}
				}
				for (int j = 0; j<nCols; j++)
				{
					if (temp.minRow[j]<18)
					{
						int flag = temp.pDC[temp.minRow[j]][j];
						for (int k = temp.minRow[j] + 1; k<nRows; ++k)
						{
							if (flag != temp.pDC[k][j])
							{
								score -= 1;
								flag = !flag;
							}
						}
					}
				}
			} else
				score = -10000;


			ao.offsetX = nowX;
			ao.shape = i;
			ao.offsetY = offset;
			list.push_back(make_pair(ao, score));
			temp.ResetTeris(*this);
		}
	}
	vector<pair<AI_OFFSET, int> > listtemp;
	stable_sort(list.begin(), list.end(), CompareAIScore);
	int i = list.size() - 1;
	int priority;
	while (i >= 0 && list[i].second == list[list.size() - 1].second) {
		if (list[i].first.offsetX <= (nCols / 2) - ptr->maxC) {
			priority = abs(nCols / 2 - ptr->maxC - list[i].first.offsetX) * 100
				+ 10 + (list[i].first.shape >= nowShape ? list[i].first.shape - nowShape : nowShape - list[i].first.shape);
		} else {
			priority = abs(nCols / 2 - ptr->maxC - list[i].first.offsetX) * 100
				+ (list[i].first.shape >= nowShape ? list[i].first.shape - nowShape : nowShape - list[i].first.shape);
		}
		listtemp.push_back(make_pair(list[i].first, priority));
		--i;
	}
	stable_sort(listtemp.begin(), listtemp.end(), CompareAIScore);
	return listtemp.at(listtemp.size() - 1).first;
}
//pass
void Teris::Init()
{
	shapeCount = 0;
	{
		char minRow[4][4] = { 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0 };
		char maxRow[4][4] = { 1, 2, 1, 0, 1, 2, 0, 0, 1, 1, 1, 0, 0, 2, 1, 0 };
		char posT[4][4][4] = { { 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0 } };
		sT[0].InitShape(posT[0], 1, 2, 0, 2, maxRow[0], minRow[0]);
		sT[1].InitShape(posT[1], 0, 2, 0, 1, maxRow[1], minRow[1]);
		sT[2].InitShape(posT[2], 0, 1, 0, 2, maxRow[2], minRow[2]);
		sT[3].InitShape(posT[3], 0, 2, 1, 2, maxRow[3], minRow[3]);
	}

	{
		char minRow[4][4] = { 0, 1, 1, 0, 1, 1, 1, 0, 0, 2, 0, 0, 1, 2, 2, 0 };
		char maxRow[4][4] = { 0, 3, 1, 0, 1, 1, 2, 0, 0, 2, 2, 0, 2, 2, 2, 0 };
		char posL[4][4][4] = { { 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
		{ 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 } };
		sL[0].InitShape(posL[0], 1, 3, 1, 2, maxRow[0], minRow[0]);
		sL[1].InitShape(posL[1], 1, 2, 0, 2, maxRow[1], minRow[1]);
		sL[2].InitShape(posL[2], 0, 2, 1, 2, maxRow[2], minRow[2]);
		sL[3].InitShape(posL[3], 1, 2, 0, 2, maxRow[3], minRow[3]);
	}

	{
		char minRow[4][4] = { 1, 1, 2, 0, 0, 1, 0, 0, 1, 1, 2, 0, 0, 1, 0, 0 };
		char maxRow[4][4] = { 1, 2, 2, 0, 0, 2, 1, 0, 1, 2, 2, 0, 0, 2, 1, 0 };
		char posZ[4][4][4] = { { 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0 } };
		sZ[0].InitShape(posZ[0], 1, 2, 0, 2, maxRow[0], minRow[0]);
		sZ[1].InitShape(posZ[1], 0, 2, 1, 2, maxRow[1], minRow[1]);
		sZ[2].InitShape(posZ[2], 1, 2, 0, 2, maxRow[2], minRow[2]);
		sZ[3].InitShape(posZ[3], 0, 2, 1, 2, maxRow[3], minRow[3]);
	}

	{
		char minRow[2][4] = { 0, 0, 0, 0, 1, 1, 1, 1 };
		char maxRow[2][4] = { 0, 0, 3, 0, 1, 1, 1, 1 };
		char posI[2][4][4] = { { 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
		{ 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 } };
		sI[0].InitShape(posI[0], 0, 3, 2, 2, maxRow[0], minRow[0]);
		sI[1].InitShape(posI[1], 1, 1, 0, 3, maxRow[1], minRow[1]);
		sI[2].InitShape(posI[0], 0, 3, 2, 2, maxRow[0], minRow[0]);
		sI[3].InitShape(posI[1], 1, 1, 0, 3, maxRow[1], minRow[1]);
	}

	{
		char minRow[4] = { 0, 1, 1, 0 };
		char maxRow[4] = { 0, 2, 2, 0 };
		char posO[4][4] = { 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0 };
		sO[0].InitShape(posO, 1, 2, 1, 2, maxRow, minRow);
		sO[1].InitShape(posO, 1, 2, 1, 2, maxRow, minRow);
		sO[2].InitShape(posO, 1, 2, 1, 2, maxRow, minRow);
		sO[3].InitShape(posO, 1, 2, 1, 2, maxRow, minRow);
	}
	{
		char minRow[2][4] = { 2, 1, 1, 0, 0, 0, 1, 0 };
		char maxRow[2][4] = { 2, 2, 1, 0, 0, 1, 2, 0 };
		char posRZ[2][4][4] = { { 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0 } };
		sRZ[0].InitShape(posRZ[0], 1, 2, 0, 2, maxRow[0], minRow[0]);
		sRZ[1].InitShape(posRZ[1], 0, 2, 1, 2, maxRow[1], minRow[1]);
		sRZ[2].InitShape(posRZ[0], 1, 2, 0, 2, maxRow[0], minRow[0]);
		sRZ[3].InitShape(posRZ[1], 0, 2, 1, 2, maxRow[1], minRow[1]);
	}
	{
		char minRow[4][4] = { 0, 1, 1, 0, 2, 2, 1, 0, 0, 1, 3, 0, 1, 1, 1, 0 };
		char maxRow[4][4] = { 0, 1, 3, 0, 2, 2, 2, 0, 0, 3, 3, 0, 2, 1, 1, 0 };
		char posRL[4][4][4] = { { 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
		{ 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0 },
		{ 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0 } };
		sRL[0].InitShape(posRL[0], 1, 3, 1, 2, maxRow[0], minRow[0]);
		sRL[1].InitShape(posRL[1], 1, 2, 0, 2, maxRow[1], minRow[1]);
		sRL[2].InitShape(posRL[2], 1, 3, 1, 2, maxRow[2], minRow[2]);
		sRL[3].InitShape(posRL[3], 1, 2, 0, 2, maxRow[3], minRow[3]);
	}
}
//pass
bool Teris::isTDLegal(POINT pt)
{
	if (!ptr) return false;
	if (nowPt.y + ptr->maxR >= nRows) {
		fState = false;
		return false;
	}
	for (char i = ptr->minC; i <= ptr->maxC; i++) {
		int tmp = minRow[nowPt.x + i] - (ptr->maxRow[i] + nowPt.y);
		if (tmp > 1) {
			continue;
		} else if (tmp < 0) {
			int j;
			for (j = ptr->minRow[i]; j <= ptr->maxRow[i]; j++) {
				if ((ptr->pos[j][i] + pDC[nowPt.y + j][nowPt.x + i])>1)
				{
					fState = false;
					return false;
				}
			}
			
		} else if (tmp == 1) {
			fState = true;
		} else if (tmp == 0){
			fState = false;
			return false;
		}
	}
	return true;
}

bool Teris::isLRLegal(POINT pt)
{
	if (!ptr) return false;
	if (rect.left - nowPt.x > ptr->minC || nowPt.x + ptr->maxC >= rect.right) return false;
	if (isTDLegal(pt)) {
		return true;
	}
	return false;
}

//pass
/// 填充方块占用的区域
void Teris::Configure()
{
	for (int i = ptr->minR; i <= ptr->maxR; i++) {
		for (int j = ptr->minC; j <= ptr->maxC; j++) {
			if (nowPt.y + i >= 0 && ptr->pos[i][j]) {
				pDC[nowPt.y + i][nowPt.x + j] = 1;
			}
		}
	}
}


void Teris::Change()
{
	Configure();
	for (int i = ptr->minC; i <= ptr->maxC; i++) {
		if (minRow[nowPt.x + i] > (nowPt.y + ptr->minRow[i])) {
			minRow[nowPt.x + i] = (short)nowPt.y + ptr->minRow[i];
			if (minRow[nowPt.x + i] < maxLine) {
				maxLine = minRow[nowPt.x + i];
			}
		}
	}
	for (int i = 0; i < nCols; i++) {
		if (minRow[i] <= 0) {
			fOver = true;
			return;
		}
	} 
	vector<int> clearLines;
	for (int i = ptr->minR; i <= ptr->maxR; i++) {
		if (isClearLine(i + nowPt.y)) {
			clearLines.push_back(i + nowPt.y);
		}
	}
	clearLineCount = clearLines.size();
	if (clearLineCount > 0) {
		int dstLine = clearLines[0];
		int srcLine = dstLine - 1;
		int endLine = maxLine;
		for (int i = 1; i < clearLines.size(); i++) {
			int line = clearLines[i];
			int cnt = line - dstLine;
			if (cnt == 1) {
				dstLine = line;
				continue;
			} else {
				while (srcLine >= endLine) {
					CopyLine(srcLine--, dstLine--);
				}
				endLine = dstLine + 1;
				dstLine = line;
				srcLine = dstLine - 1;
			}
		}
		while (srcLine >= endLine) {
			CopyLine(srcLine--, dstLine--);
		}
		for (int i = maxLine; i < clearLines.size() + maxLine; i++) {
			for (int j = 0; j < nCols; j++) {
				pDC[i][j] = 0;
			}
		}
		initminRow();
		ComputeScore(clearLines.size());
	}
	nowShape = nextShape;
	ptr = nextPtr;

	nowPt.x = (nCols) / 2 - ptr->maxC;
	nowPt.y = -(ptr->maxR + 1);
	fState = false;
	SetNextShape();
}

bool Teris::isClearLine(int line)
{
	if (line < 0) return false;
	for (int i = 0; i < nCols; i++) {
		if (!pDC[line][i]) return false;
	}
	return true;
}

void Teris::CopyLine(int srcLine, int dstLine)
{
	if (srcLine < 0 || dstLine < 0) return;
	for (int i = 0; i < nCols; i++) {
		pDC[dstLine][i] = pDC[srcLine][i];
	}
}

void Teris::initminRow()
{
	for (int i = 0; i < nCols; i++) {
		int j;
		for (j = 0; j < nRows; j++) {
			if (pDC[j][i]) {
				minRow[i] = j;
				break;
			}
		}
		minRow[i] = j;
	}
	
}

void Teris::SetNextShape()
{
	++shapeCount;
	nextShape = rand() % 4;
	switch (rand() % (RZ + 1))
	{
	case T:
		nextPtr = sT + nextShape;
		break;
	case L:
		nextPtr = sL + nextShape;
		break;
	case Z:
		nextPtr = sZ + nextShape;
		break;
	case I:
		nextPtr = sI + nextShape;
		break;
	case O:
		nextPtr = sO + nextShape;
		break;
	case RL:
		nextPtr = sRL + nextShape;
		break;
	case RZ:
		nextPtr = sRZ + nextShape;
		break;
	default:
		nextPtr = NULL;
	}
}

bool CompareAIScore(const pair<AI_OFFSET, int>& l, const pair<AI_OFFSET, int>& r)
{
	return l.second < r.second;
}
