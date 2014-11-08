/*
 * Author: Sergey Kolosov <kolosov@gmail.com>
 */

#ifndef GTK_GECKO_SRV_H
#define GTK_GECKO_SRV_H

#include <iostream>

#define MAX_BUF_SIZE 1024*1024

class GeckoSrvCmdParser {
public:
	const std::string Q_LOAD_URL_CMD_PREFIX = "load";
	const std::string Q_GET_TEXT_CONTENT_CMD = "text_content";
	bool ParseCmd(std::string aCmd);
	std::string GetTextContent();
};

#endif //GTK_GECKO_SRV_H
