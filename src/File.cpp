/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:11:07 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/30 23:10:19 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/File.hpp"

File::File() {};
File::File( std::string Name, std::string Path, std::string Sender, std::string Receiver ): Name(Name), Path(Path), sender(Sender), receiver(Receiver) {};
File::File(const File &x) 
{
	*this = x;
};
File::~File() {};

File & File::operator=( const File &rhs )
{
	if (this == &rhs)
		return (*this);
	this->Name = rhs.Name;
	this->Path = rhs.Path;
	this->sender = rhs.sender;
	this->receiver = rhs.receiver;
	return (*this);
};

std::string Server::_sendFile(Request request, int i) 
{
    if (!this->_clients[i]->getRegistered()) 
    {
        return _printMessage("451", this->_clients[i]->getNickName(), ":You have not registered");
    }

    if (request.args.size() < 2) 
    {
        return _printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters, usage: SENDFILE <nick> <file path>");
    }

    int targetFd = _findFdByNickName(request.args[0]);
    if (targetFd == USERNOTFOUND) 
    {
        return _printMessage("401", this->_clients[i]->getNickName(), request.args[0] + " :No such nick/channel");
    }

    if (this->_clients[i]->getNickName() == request.args[0]) 
    {
        return _printMessage("997", this->_clients[i]->getNickName(), ":You cannot send a file to yourself");
    }

    std::fstream ifs(request.args[1].c_str(), std::fstream::in);
    if (ifs.fail()) 
    {
        return _printMessage("999", this->_clients[i]->getNickName(), ":Invalid file path");
    }

    size_t pos = request.args[1].find_last_of('/');
    std::string filename = (pos == std::string::npos) ? request.args[1] : request.args[1].substr(pos + 1);
    File file(filename, request.args[1], this->_clients[i]->getNickName(), request.args[0]);

    std::map<std::string, File>::iterator fileIt = this->_files.find(filename);
    if (fileIt != this->_files.end()) 
    {
        this->_files.erase(fileIt);
    }
    this->_files.insert(std::make_pair(filename, file));
    _privToUser(request.args[0], this->_clients[i]->getNickName() + " wants to send you a file called " + filename + ".", "NOTICE", i);
    return _printMessage("200", this->_clients[i]->getNickName(), ":Notice sent to " + request.args[0] + " about the file " + filename);
}

std::string Server::_getFile(Request request, int i) 
{
    if (!this->_clients[i]->getRegistered()) 
    {
        return _printMessage("451", this->_clients[i]->getNickName(), ":You have not registered");
    }

    if (request.args.size() < 2) 
    {
        return _printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters, usage: GETFILE <file name> <file path>");
    }

    std::map<std::string, File>::iterator fileIt = this->_files.find(request.args[0]);
    if (fileIt == this->_files.end()) 
    {
        return _printMessage("995", this->_clients[i]->getNickName(), ":No such file on the server");
    }

    File file = fileIt->second;
    if (file.receiver != this->_clients[i]->getNickName()) 
    {
        return _printMessage("994", this->_clients[i]->getNickName(), ":Permission Denied");
    }

    std::string outputFilePath = request.args[1] + "/" + request.args[0];
    std::fstream ofs(outputFilePath.c_str(), std::fstream::out);
    std::fstream ifs(file.Path.c_str(), std::fstream::in);

    if (!ofs.is_open() || !ifs.is_open()) 
    {
        return _printMessage("998", this->_clients[i]->getNickName(), ":Error opening file for transfer");
    }
    ofs << ifs.rdbuf();
    ifs.close();
    ofs.close();
    this->_files.erase(file.Name);
    
    return _printMessage("200", this->_clients[i]->getNickName(), ":File " + request.args[0] + " received successfully");
}
