/***
	MIT License

	Copyright (c) 2018 NUMERARE

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

	Parts of this file are originally Copyright (c) 2012-2017 The CryptoNote developers, The Bytecoin developers
***/

#pragma once

#include <iostream>
#include <map>
#include <string>

namespace CryptoNote {
  class HttpRequest {
  public:
    typedef std::map<std::string, std::string> Headers;

    const std::string& getMethod() const;
    const std::string& getUrl() const;
    const Headers& getHeaders() const;
    const std::string& getBody() const;

    void addHeader(const std::string& name, const std::string& value);
    void setBody(const std::string& b);
    void setUrl(const std::string& uri);

  private:
    friend class HttpParser;

    std::string method;
    std::string url;
    Headers headers;
    std::string body;

    friend std::ostream& operator<<(std::ostream& os, const HttpRequest& resp);
    std::ostream& printHttpRequest(std::ostream& os) const;
  };

  inline std::ostream& operator<<(std::ostream& os, const HttpRequest& resp) {
    return resp.printHttpRequest(os);
  }
}
