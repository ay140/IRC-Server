/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:11:07 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/26 12:02:58 by ayman_marzo      ###   ########.fr       */
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
        // 451: ERR_NOTREGISTERED - Client must register before performing this action
        return _printMessage("451", this->_clients[i]->getNickName(), ":You have not registered");
    }

    if (request.args.size() < 2) 
    {
        // 461: ERR_NEEDMOREPARAMS - Not enough parameters provided for the command
        return _printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters");
    }

    int targetFd = _findFdByNickName(request.args[0]);
    if (targetFd == USERNOTFOUND) 
    {
        // 401: ERR_NOSUCHNICK - Target nickname or channel does not exist
        return _printMessage("401", this->_clients[i]->getNickName(), request.args[0] + " :No such nick/channel");
    }

    std::fstream ifs(request.args[1].c_str(), std::fstream::in);
    if (ifs.fail()) 
    {
        // 999: ERR_INVALIDFILEPATH - Custom error for invalid file path
        return _printMessage("999", this->_clients[i]->getNickName(), ":Invalid file path");
    }

    size_t pos = request.args[1].find_last_of('/');
    std::string filename = (pos == std::string::npos) ? request.args[1] : request.args[1].substr(pos + 1);
    File file(filename, request.args[1], this->_clients[i]->getNickName(), request.args[0]);

    if (this->_files.find(filename) != this->_files.end()) 
    {
        // 996: ERR_FILEEXISTS - Custom error for file already exists
        return _printMessage("996", this->_clients[i]->getNickName(), ":File by this name already exists");
    }

    this->_files.insert(std::make_pair(filename, file));
    _privToUser(request.args[0], this->_clients[i]->getNickName() + " wants to send you a file called " + filename + ".", "NOTICE", i);
    return "";
}


std::string Server::_getFile(Request request, int i) 
{
    if (!this->_clients[i]->getRegistered()) 
    {
        // 451: ERR_NOTREGISTERED - Client must register before performing this action
        return _printMessage("451", this->_clients[i]->getNickName(), ":You have not registered");
    }

    if (request.args.size() < 2) 
    {
        // 461: ERR_NEEDMOREPARAMS - Not enough parameters provided for the command
        return _printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters");
    }

    std::map<std::string, File>::iterator fileIt = this->_files.find(request.args[0]);
    if (fileIt == this->_files.end()) 
    {
        // 995: ERR_NOFILE - Custom error for no such file on the server
        return _printMessage("995", this->_clients[i]->getNickName(), ":No such file on the server");
    }

    File file = fileIt->second;
    if (file.receiver != this->_clients[i]->getNickName()) 
    {
        // 994: ERR_PERMISSIONDENIED - Custom error for permission denied
        return _printMessage("994", this->_clients[i]->getNickName(), ":Permission Denied");
    }

    std::string outputFilePath = request.args[1] + "/" + request.args[0];
    std::fstream ofs(outputFilePath.c_str(), std::fstream::out);
    std::fstream ifs(file.Path.c_str(), std::fstream::in);

    if (!ofs.is_open() || !ifs.is_open()) 
    {
        // 998: ERR_FILETRANSFER - Custom error for error opening file for transfer
        return _printMessage("998", this->_clients[i]->getNickName(), ":Error opening file for transfer");
    }
    ofs << ifs.rdbuf();
    ifs.close();
    ofs.close();
    this->_files.erase(file.Name);
    return "";
}
