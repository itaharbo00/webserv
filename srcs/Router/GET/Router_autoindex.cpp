/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_autoindex.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 18:40:00 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/15 18:38:02 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>

struct FileInfo {
	std::string name;
	bool isDirectory;
	off_t size;
	time_t mtime;
};

static bool compareFileInfo(const FileInfo &a, const FileInfo &b)
{
	// Répertoires en premier, puis fichiers
	if (a.isDirectory != b.isDirectory)
		return a.isDirectory;
	// Ordre alphabétique
	return a.name < b.name;
}

static std::string formatSize(off_t size)
{
	if (size < 1024)
	{
		std::stringstream ss;
		ss << size << " B";
		return ss.str();
	}
	else if (size < 1024 * 1024)
	{
		std::stringstream ss;
		ss << (size / 1024) << " KB";
		return ss.str();
	}
	else if (size < 1024 * 1024 * 1024)
	{
		std::stringstream ss;
		ss << (size / (1024 * 1024)) << " MB";
		return ss.str();
	}
	else
	{
		std::stringstream ss;
		ss << (size / (1024 * 1024 * 1024)) << " GB";
		return ss.str();
	}
}

static std::string formatTime(time_t mtime)
{
	char buffer[64];
	struct tm *tm_info = localtime(&mtime);
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
	return std::string(buffer);
}

std::string	Router::generateDirectoryListing(const std::string &dirPath, const std::string &uri)
{
	DIR *dir = opendir(dirPath.c_str());
	if (!dir)
		throw std::runtime_error("Cannot open directory");

	std::vector<FileInfo> files;
	struct dirent *entry;
	
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		
		// Ignorer . (répertoire actuel)
		if (name == ".")
			continue;
		
		std::string fullPath = dirPath + "/" + name;
		struct stat st;
		
		if (stat(fullPath.c_str(), &st) == 0)
		{
			FileInfo info;
			info.name = name;
			info.isDirectory = S_ISDIR(st.st_mode);
			info.size = st.st_size;
			info.mtime = st.st_mtime;
			files.push_back(info);
		}
	}
	
	closedir(dir);
	
	// Trier : répertoires d'abord, puis ordre alphabétique
	std::sort(files.begin(), files.end(), compareFileInfo);
	
	// Générer le HTML
	std::stringstream html;
	html << "<!DOCTYPE html>\n";
	html << "<html lang=\"en\">\n";
	html << "<head>\n";
	html << "    <meta charset=\"UTF-8\">\n";
	html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
	html << "    <title>Index of " << uri << "</title>\n";
	html << "    <style>\n";
	html << "        body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }\n";
	html << "        h1 { color: #333; border-bottom: 2px solid #667eea; padding-bottom: 10px; }\n";
	html << "        table { width: 100%; border-collapse: collapse; background: white; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n";
	html << "        th { background: #667eea; color: white; padding: 12px; text-align: left; }\n";
	html << "        td { padding: 10px; border-bottom: 1px solid #ddd; }\n";
	html << "        tr:hover { background: #f0f0f0; }\n";
	html << "        a { color: #667eea; text-decoration: none; }\n";
	html << "        a:hover { text-decoration: underline; }\n";
	html << "        .dir { font-weight: bold; }\n";
	html << "        .dir::before { content: '📁 '; }\n";
	html << "        .file::before { content: '📄 '; }\n";
	html << "    </style>\n";
	html << "</head>\n";
	html << "<body>\n";
	html << "    <h1>Index of " << uri << "</h1>\n";
	html << "    <table>\n";
	html << "        <thead>\n";
	html << "            <tr>\n";
	html << "                <th>Name</th>\n";
	html << "                <th>Size</th>\n";
	html << "                <th>Last Modified</th>\n";
	html << "            </tr>\n";
	html << "        </thead>\n";
	html << "        <tbody>\n";
	
	for (size_t i = 0; i < files.size(); ++i)
	{
		const FileInfo &file = files[i];
		std::string href = uri;
		if (href[href.length() - 1] != '/')
			href += "/";
		href += file.name;
		if (file.isDirectory)
			href += "/";
		
		html << "            <tr>\n";
		html << "                <td><a href=\"" << href << "\" class=\"";
		html << (file.isDirectory ? "dir" : "file") << "\">";
		html << file.name;
		if (file.isDirectory)
			html << "/";
		html << "</a></td>\n";
		html << "                <td>" << (file.isDirectory ? "-" : formatSize(file.size)) << "</td>\n";
		html << "                <td>" << formatTime(file.mtime) << "</td>\n";
		html << "            </tr>\n";
	}
	
	html << "        </tbody>\n";
	html << "    </table>\n";
	html << "</body>\n";
	html << "</html>\n";
	
	return html.str();
}
