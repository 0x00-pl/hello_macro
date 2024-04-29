#ifndef PL_SUBSTR
#define PL_SUBSTR

#include "pl_error.h"

typedef struct _pl_substr{
	const char* beg;
	size_t length;
}pl_substr;

pl_substr pl_substr_init(pl_substr* obj, const char* _beg, size_t _len){
	obj->beg= _beg;
	obj->length= _len;
	return *obj;
}

size_t pl_substr_indexof(const pl_substr* obj, const char* p){
	return p - obj->beg;
}

pl_substr pl_substr_formstr(pl_substr* obj, const char* src){
	obj->beg= src;
	obj->length= strlen(src);
	return *obj;
}

char* pl_substr_tostr(const pl_substr* obj, char* dest){
	memcpy(dest, obj->beg, obj->length);
	dest[obj->length]= '\0';
	return dest;
}

char* pl_substr_tostr_safe(const pl_substr* obj, char* dest, const char* end){
	if(dest+obj->length >= end) pl_error("pl: not enough buffer. 数组越界");
	return	pl_substr_tostr(obj, dest);
}

char pl_substr_head(const pl_substr* obj){
	return obj->length? *obj->beg: '\0';
}

pl_substr pl_substr_pophead(pl_substr* obj){
	if(obj->length){
		--obj->length;
		++obj->beg;
	}
	return *obj;
}
pl_substr pl_substr_pophead_n(pl_substr* obj, size_t n){
	if(obj->length > n){
		obj->length-= n;
		obj->beg+= n;
	}else{
		obj->beg+= obj->length;
		obj->length= 0;
	}
	return *obj;
}

pl_substr pl_substr_pophead_logic(pl_substr* obj){
	if(pl_substr_head(obj)== '\\') {
		pl_substr_pophead(obj);// "\\"
		if(pl_substr_head(obj)== '\r') pl_substr_pophead(obj);
		if(pl_substr_head(obj)== '\n') {pl_substr_pophead(obj);return *obj;}// "\r\n" or "\n"
	}
	pl_substr_pophead(obj);// (no "\\") or ("\\ \t" etc.)
	return *obj;
}

pl_substr pl_substr_substr(const pl_substr* obj, size_t begin, size_t length){
	pl_substr ret;
	begin= begin > obj->length? obj->length: begin;
	length= length+begin > obj->length? obj->length-begin: length;
	pl_substr_init(&ret, obj->beg+begin, length); 
	return ret;
}

size_t pl_substr_tostr_as_str(pl_substr obj, char* dest, const char* end){
	char* old= dest;
	char c;
	while(dest<end && obj.length){
		switch(c=pl_substr_head(&obj)){
		case '\"':
			*dest++ = '\\';
			if(dest<end)*dest++ = '\"';
			break;
		case '\\':
			*dest++ = '\\';
			if(dest<end)*dest++ = '\\';
		default:
			*dest++ = c;
		}
		pl_substr_pophead(&obj);
	}
	if(dest<end) *dest= '\0'; 
	return dest-old;
}

#endif//PL_SUBSTR