/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Cryto.h
 * Author: cpu10855
 *
 * Created on July 22, 2019, 3:52 PM
 */

#ifndef CRYTO_H
#define CRYTO_H

#include "Poco/Foundation.h"
#include "Poco/DigestEngine.h"
#include "Poco/Crypto/DigestEngine.h"

class SHA256Engine : public Poco::Crypto::DigestEngine
{
public:
    enum
    {
        BLOCK_SIZE = 64,
        DIGEST_SIZE = 32
    };

    SHA256Engine(): DigestEngine("SHA256")
    {
    }
    
};

#endif /* CRYTO_H */

