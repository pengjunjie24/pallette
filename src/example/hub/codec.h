/*************************************************************************
  > File Name: codec.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年03月05日 星期二 19时55分08秒
 ************************************************************************/

#ifndef PALLETTE_EXAMPLES_HUB_CODEC_H
#define PALLETTE_EXAMPLES_HUB_CODEC_H

#include "../../base/Buffer.h"
#include <string>

namespace pubsub
{
	enum ParseResult
	{
		PARSE_ERROR,
		PARSE_SUCCESS,
		PARSE_CONTINUE,
	};

	ParseResult parseMessage(pallette::Buffer* buf,
		std::string* cmd,
		std::string* topic,
		std::string* content);
}

#endif