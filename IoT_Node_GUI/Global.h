#pragma once
#ifndef GLOBAL
#define GLOBAL
#include <QTimer>
#include <QString>
#include <QTableWidget>
#include "myitem.h"
#include "myline.h"
#include "TextItem.h"
#include "Consensus.h"
struct FATestStruct
{
	unsigned int start;
	unsigned int end;
	unsigned int type;
	unsigned int index;
	unsigned int transRange;
};
struct FATimeItem
{
	int type;
	unsigned int time;
};
struct FAItem
{
	unsigned int index=0;
	int action = -1;
	unsigned int startTime=0;
	unsigned int endTime=0;
};
inline bool large_sort(FATimeItem *a, FATimeItem *b)
{
	return a->time > b->time;
}
inline bool less_sort(FATimeItem *a, FATimeItem *b)
{
	return a->time < b->time;
}
extern QTableWidget *TestWidget;
extern unsigned int viewNumber;
#endif
