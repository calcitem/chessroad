/*
eleeye.cpp - Source Code for ElephantEye, Part IX

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.3, Last Modified: Mar. 2012
Copyright (C) 2004-2012 www.xqbase.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <stdarg.h>
#include "base2.h"
#include "parse.h"
#include "ucci.h"
#include "pregen.h"
#include "position.h"
#include "hash.h"
#include "search.h"
#include "command-channel.h"

const int INTERRUPT_COUNT = 4096; // �������ɽ�������ж�

void PrintLn(const char *sz, ...) {

    va_list args;

    va_start(args, sz);

    char buffer[256] = {0};
    vsprintf(buffer, sz, args);

    va_end(args);

    CommandChannel *channel = CommandChannel::getInstance();
    while (!channel->pushResponse(buffer)) Idle();
}

#ifdef WIN32
LARGE_INTEGER
getFILETIMEoffset()
{
    SYSTEMTIME s;
    FILETIME f;
    LARGE_INTEGER t;

    s.wYear = 1970;
    s.wMonth = 1;
    s.wDay = 1;
    s.wHour = 0;
    s.wMinute = 0;
    s.wSecond = 0;
    s.wMilliseconds = 0;
    SystemTimeToFileTime(&s, &f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
    return (t);
}

int
clock_gettime(int X, struct timeval *tv)
{
    LARGE_INTEGER           t;
    FILETIME            f;
    double                  microseconds;
    static LARGE_INTEGER    offset;
    static double           frequencyToMicroseconds;
    static int              initialized = 0;
    static BOOL             usePerformanceCounter = 0;

    if (!initialized) {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
        if (usePerformanceCounter) {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        } else {
            offset = getFILETIMEoffset();
            frequencyToMicroseconds = 10.;
        }
    }
    if (usePerformanceCounter) QueryPerformanceCounter(&t);
    else {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
    }

    t.QuadPart -= offset.QuadPart;
    microseconds = (double)t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = microseconds;
    tv->tv_sec = t.QuadPart / 1000000;
    tv->tv_usec = t.QuadPart % 1000000;
    return (0);
}

int main(void) {
#else
int engineMain(void) {
#endif
  int i;
  bool bPonderTime;
  UcciCommStruct UcciComm;
  PositionStruct posProbe;

  if (BootLine() != UCCI_COMM_UCCI) {
    return 0;
  }
  LocatePath(Search.szBookFile, "BOOK.DAT");
  bPonderTime = false;
  PreGenInit();
  NewHash(24); // 24=16MB, 25=32MB, 26=64MB, ...
  Search.pos.FromFen(cszStartFen);
  Search.pos.nDistance = 0;
  Search.pos.PreEvaluate();
  Search.nBanMoves = 0;
  Search.bQuit = Search.bBatch = Search.bDebug = false;
  Search.bUseHash = Search.bUseBook = Search.bNullMove = Search.bKnowledge = true;
  Search.bIdle = false;
  Search.nCountMask = INTERRUPT_COUNT - 1;
  Search.nRandomMask = 0;
  Search.rc4Random.InitRand();
  PrintLn("id name ElephantEye");
  PrintLn("id version 3.31");
  PrintLn("id copyright 2004-2016 www.xqbase.com");
  PrintLn("id author ElephantEye Development Team");
  PrintLn("id user ElephantEye Test Team");
  PrintLn("option usemillisec type check default true");
  PrintLn("option promotion type check default false");
  PrintLn("option batch type check default false");
  PrintLn("option debug type check default false");
  PrintLn("option ponder type check default false");
  PrintLn("option usehash type check default true");
  PrintLn("option usebook type check default true");
  PrintLn("option bookfiles type string default %s\n", Search.szBookFile);
  PrintLn("option hashsize type spin min 16 max 1024 default 16");
  PrintLn("option idle type combo var none var small var medium var large default none");
  PrintLn("option pruning type combo var none var small var medium var large default large");
  PrintLn("option knowledge type combo var none var small var medium var large default large");
  PrintLn("option randomness type combo var none var tiny var small var medium var large var huge default none");
  PrintLn("option newgame type button");
  PrintLn("ucciok");

  // �����ǽ���ָ����ṩ�Բߵ�ѭ����
  while (!Search.bQuit) {
    switch (IdleLine(UcciComm, Search.bDebug)) {
    case UCCI_COMM_ISREADY:
      PrintLn("readyok");
      break;
    case UCCI_COMM_STOP:
      PrintLn("nobestmove");
      break;
    case UCCI_COMM_POSITION:
      BuildPos(Search.pos, UcciComm);
      Search.pos.nDistance = 0;
      Search.pos.PreEvaluate();
      Search.nBanMoves = 0;
      break;
    case UCCI_COMM_BANMOVES:
      Search.nBanMoves = UcciComm.nBanMoveNum;
      for (i = 0; i < UcciComm.nBanMoveNum; i ++) {
        Search.wmvBanList[i] = COORD_MOVE(UcciComm.lpdwBanMovesCoord[i]);
      }
      break;
    case UCCI_COMM_SETOPTION:
      switch (UcciComm.Option) {
      case UCCI_OPTION_PROMOTION:
        PreEval.bPromotion = UcciComm.bCheck;
        break;
      case UCCI_OPTION_BATCH:
        Search.bBatch = UcciComm.bCheck;
        break;
      case UCCI_OPTION_DEBUG:
        Search.bDebug = UcciComm.bCheck;
        break;
      case UCCI_OPTION_PONDER:
        bPonderTime = UcciComm.bCheck;
        break;
      case UCCI_OPTION_USEHASH:
        Search.bUseHash = UcciComm.bCheck;
        break;
      case UCCI_OPTION_USEBOOK:
        Search.bUseBook = UcciComm.bCheck;
        break;
      case UCCI_OPTION_BOOKFILES:
        if (AbsolutePath(UcciComm.szOption)) {
          strcpy(Search.szBookFile, UcciComm.szOption);
        } else {
          LocatePath(Search.szBookFile, UcciComm.szOption);
        }
        break;
      case UCCI_OPTION_HASHSIZE:
        DelHash();
        i = 19; // С��1������0.5M�û���
        while (UcciComm.nSpin > 0) {
          UcciComm.nSpin /= 2;
          i ++;
        }
        NewHash(MAX(i, 24)); // ��С���û�����Ϊ16M
        break;
      case UCCI_OPTION_IDLE:
        switch (UcciComm.Grade) {
        case UCCI_GRADE_NONE:
          Search.bIdle = false;
          Search.nCountMask = INTERRUPT_COUNT - 1;
          break;
        case UCCI_GRADE_SMALL:
          Search.bIdle = true;
          Search.nCountMask = INTERRUPT_COUNT / 4 - 1;
          break;
        case UCCI_GRADE_MEDIUM:
          Search.bIdle = true;
          Search.nCountMask = INTERRUPT_COUNT / 16 - 1;
          break;
        case UCCI_GRADE_LARGE:
          Search.bIdle = true;
          Search.nCountMask = INTERRUPT_COUNT / 64 - 1;
          break;
        default:
          break;
        }
        break;
      case UCCI_OPTION_PRUNING:
        Search.bNullMove = (UcciComm.Grade != UCCI_GRADE_NONE);
        break;
      case UCCI_OPTION_KNOWLEDGE:
        Search.bKnowledge = (UcciComm.Grade != UCCI_GRADE_NONE);
        break;
      case UCCI_OPTION_RANDOMNESS:
        switch (UcciComm.Grade) {
        case UCCI_GRADE_NONE:
          Search.nRandomMask = 0;
          break;
        case UCCI_GRADE_TINY:
          Search.nRandomMask = 1;
          break;
        case UCCI_GRADE_SMALL:
          Search.nRandomMask = 3;
          break;
        case UCCI_GRADE_MEDIUM:
          Search.nRandomMask = 7;
          break;
        case UCCI_GRADE_LARGE:
          Search.nRandomMask = 15;
          break;
        case UCCI_GRADE_HUGE:
          Search.nRandomMask = 31;
          break;
        default:
          break;
        }
        break;
      default:
        break;
      }
      break;
    case UCCI_COMM_GO:
      Search.bPonder = UcciComm.bPonder;
      Search.bDraw = UcciComm.bDraw;
      switch (UcciComm.Go) {
      case UCCI_GO_DEPTH:
        Search.nGoMode = GO_MODE_INFINITY;
        Search.nNodes = 0;
        SearchMain(UcciComm.nDepth);
        break;
      case UCCI_GO_NODES:
        Search.nGoMode = GO_MODE_NODES;
        Search.nNodes = UcciComm.nNodes;
        SearchMain(UCCI_MAX_DEPTH);
        break;
      case UCCI_GO_TIME_MOVESTOGO:
      case UCCI_GO_TIME_INCREMENT:
        Search.nGoMode = GO_MODE_TIMER;
        if (UcciComm.Go == UCCI_GO_TIME_MOVESTOGO) {
          // ����ʱ���ƣ���ʣ��ʱ��ƽ�����䵽ÿһ������Ϊ�ʵ�ʱ�ޡ�
          // ʣ�ಽ����1��5�����ʱ��������ʣ��ʱ���100%��90%��80%��70%��60%��5���϶���50%
          Search.nProperTimer = UcciComm.nTime / UcciComm.nMovesToGo;
          Search.nMaxTimer = UcciComm.nTime * MAX(5, 11 - UcciComm.nMovesToGo) / 10;
        } else {
          // ���ڼ�ʱ�ƣ�������ֻ���20�غ��ڽ��������ƽ��ÿһ�����ʵ�ʱ�ޣ����ʱ����ʣ��ʱ���һ��
          Search.nProperTimer = UcciComm.nTime / 20 + UcciComm.nIncrement;
          Search.nMaxTimer = UcciComm.nTime / 2;
        }
        // ����Ǻ�̨˼����ʱ�������ԣ���ô�ʵ�ʱ����Ϊԭ����1.25��
        Search.nProperTimer += (bPonderTime ? Search.nProperTimer / 4 : 0);
        Search.nMaxTimer = MIN(Search.nMaxTimer, Search.nProperTimer * 10);
        SearchMain(UCCI_MAX_DEPTH);
        break;
      default:
        break;
      }
      break;
    case UCCI_COMM_PROBE:
      BuildPos(posProbe, UcciComm);
      if (!PopHash(posProbe)) {
        PopLeaf(posProbe);
      }
      break;
    case UCCI_COMM_QUIT:
      Search.bQuit = true;
      break;
    default:
      break;
    }
  }
  DelHash();
  PrintLn("bye");
  return 0;
}
