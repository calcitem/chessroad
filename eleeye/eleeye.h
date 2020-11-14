//
//  eleeye.h
//  Runner
//
//  Created by 贺照云 on 2020/3/18.
//

#ifndef eleeye_h
#define eleeye_h

#ifdef WIN32
//#define strcasestr _strcasestr
#define strnicmp _strnicmp
#endif

void PrintLn(const char *sz, ...);

int engineMain();

#endif /* eleeye_h */
