#include "athttp.h"
#include "debug.h"

http_request_t request_buffer;

http_request_t *parseHttpRequest(String request) {
  // process:
  // scan for \r\n+IPD,<link> and ignore everything up to and including next ":"
  // scan for / or \r or \n.
  // if get \r or \n then invalid
  // if /:
  //   ignore /
  //   parse words separated by /, stopping at white space (' ', '?', '&', \r, \n)
  //   populate words
  //   set count
  //   set valid

  // Mark result as invalid to start.
  request_buffer.valid = false;
  request_buffer.numUrlParts = 0;
  request_buffer.ipdLink[0] = '\0';

  char *p = &request[0];

  // Process +IPD, which is of the form +IPD,1,25 where 1 is the link number and 25
  // is the byte count of the request.
  // First, find +IPD. 
  p = strstr(p, "+IPD,");
  if (p == NULL) {
    debugOut("No +IPD");
    // no data found
    return NULL;
  }
  p += 5;
  char *q = request_buffer.ipdLink;
  int i = 0;
  while (*p != ',' && i < ATHTTP_IPD_LENGTH) {
    *q++ = *p++;
  }
  *q = '\0';
  // debugOut("IPD: ");
  // debugOut(request_buffer.ipdLink);

  // ignore everything up to and including the ":"
  p = strstr(p, ":");
  if (p == NULL) {
    return NULL;
  }
  p++;

  // debugOut("After +IPD scan:");
  // debugOut(p);
  // debugOut("=====");

  // Scan for /, \r or \n
  while (*p && *p != '/' && *p != '\r' && *p != '\n') p++;

  // debugOut("Should be looking at leading '/':");
  // debugOut(p);
  // debugOut("=====");

  if (*p != '/') {
    // request ended prematurely
    debugOut("request ended prematurely");
    return NULL;
  }

  while (true) {
    // debugOut("Start of loop:");
    // debugOut(p);
    // debugOut("=====");

    char *q = request_buffer.numUrlParts < ATHTTP_MAX_URL_PARTS ?
        request_buffer.urlParts[request_buffer.numUrlParts] :
        NULL;
    int copiedCount = 0;
    p++; // skip '/'
    while (*p && *p != '/' && *p != '\r' && *p != '\n' && *p != '?' && *p != '&' && *p != ' ') {
      if (q && copiedCount < (ATHTTP_MAX_URL_PART_LENGTH-1)) {
        // we're within limits
        *q++ = *p++;
        copiedCount++;
      } else {
        // we're not in limits, just ignore
        p++;
      }
    }
    if (q) {
        *q = '\0';
        request_buffer.numUrlParts++;
    }

    // debugOut("After scanning url part:");
    // debugOut(p);
    // debugOut("=====");

    if (*p == ' ') {
      p++;
      // look for HTTP
      char *q = strstr(p, "HTTP/");
      if (q) {
        request_buffer.isHTTP = true;
        request_buffer.valid = true;
        return &request_buffer;
      }
    }

    if (*p != '/') {
        request_buffer.valid = true;
        return &request_buffer;
    }
  }
}