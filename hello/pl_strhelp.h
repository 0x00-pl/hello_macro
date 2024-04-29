#ifndef PL_STRHELP
#define PL_STRHELP


#include "pl_substr.h"
#include "pl_vector.h"

PL_VECTOR_DEFALL(pl_substr);

int pl_str_compare(pl_substr a, pl_substr b){
	while(b.length--)
		if(!a.length-- || *a.beg++ != *b.beg++) return -1;
	return 0;
}

int pl_str_is_ablechar(char c){
	return (c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='_';
}

pl_vector_Tpl_substr pl_str_findall(pl_substr src, pl_substr key){
	pl_vector_Tpl_substr ret;
	pl_substr temp;
	pl_vector_init_Tpl_substr(&ret);
	while(src.length){
		if( pl_str_compare(src, key) == 0){
			pl_substr_init(&temp, src.beg, key.length);
			pl_vector_push_Tpl_substr(&ret, &temp);
		}
		pl_substr_pophead_logic(&src);
	}
	return ret;
}


pl_substr pl_str_jump_text(pl_substr s){
	if(pl_substr_head(&s)== '\'') {
		pl_substr_pophead(&s); // '\''
		pl_substr_pophead_logic(&s); // 'c'
		pl_substr_pophead(&s); // '\''
	}else if(pl_substr_head(&s)== '\"'){
		pl_substr_pophead(&s); // '\"'
		while(pl_substr_head(&s)!= '\"') 
			pl_substr_pophead_logic(&s); // "str"
		pl_substr_pophead(&s); // '\"'
	}
	return s;
}

pl_substr pl_str_get_text(pl_substr s){
	pl_substr ns= s;
	ns= pl_str_jump_text(ns);
	//[text[string]]
	//<-----s------>
	//_____<---ns-->
	return pl_substr_substr(&s,
		0,
		s.length- ns.length
		);
}

pl_substr pl_str_jump_parentheses(pl_substr s){
	int lvl;
	lvl= 0;

	while(1){
		switch(pl_substr_head(&s)){
		case '(':
			++lvl;
			pl_substr_pophead(&s);
			break;
		case ')':
			--lvl;
			pl_substr_pophead(&s);
			if(lvl<= 0){
				return s;
			}
			break;
		case '\"': case '\'':
			s= pl_str_jump_text(s);
			break;
		default:
			pl_substr_pophead(&s);
			if( !s.length )
				return s;
		}
	}
}


pl_substr pl_str_get_parentheses(pl_substr s){
	pl_substr ns= s;
	ns= pl_str_jump_parentheses(ns);
	//[text[string]]
	//<-----s------>
	//_____<---ns-->
	return pl_substr_substr(&s,
		0,
		s.length- ns.length
		);
}


pl_substr pl_str_get_symbol(pl_substr s){
	size_t i;
	for(i=0; i<s.length && pl_str_is_ablechar(s.beg[i]);i++);

	s.length= i;
	return s;
}
pl_substr pl_str_jump_symbol(pl_substr s){
	pl_substr_pophead_n(&s, pl_str_get_symbol(s).length);
	return s;
}

pl_substr pl_str_jump_until_char(pl_substr s, char c){
	// [abc[def]]
	// <---old-->
	// ____<new->
	while(s.length){
		s= pl_str_jump_text(s);
		if(pl_substr_head(&s)== c) break;
		pl_substr_pophead_logic(&s);
	}
	pl_substr_pophead(&s);
	return s;
}
pl_substr pl_str_get_until_char(pl_substr s, char c){
	return pl_substr_substr(
		&s,
		0,
		pl_substr_indexof(&s,pl_str_jump_until_char(s,c).beg)
		);
}
pl_vector_Tpl_substr pl_str_cut_with_level(pl_substr str){
	int lvl;
	pl_substr temp;
	pl_vector_Tpl_substr ret;
	pl_vector_init_Tpl_substr(&ret);
	lvl= 0;
	temp.beg= str.beg;
	for(; str.length; --str.length){
		switch(*str.beg){
		//lvlup
		case '(':
			++str.beg;
			++lvl;
			break;
		//lvldown
		case ')':
			--lvl;
			if(lvl<0){
				//push args
				temp.length= str.beg- temp.beg;
				pl_vector_push_Tpl_substr(&ret, &temp);
				return ret;
			}
			++str.beg;
			break;
		//args
		case ',':
			if(lvl==0){
				//push args
				temp.length= str.beg- temp.beg;
				pl_vector_push_Tpl_substr(&ret, &temp);
				++str.beg;//skip ','
				temp.beg= str.beg;
			}else{
				++str.beg;
			}
			break;
		//text
		case '\"':
			++str.beg;//skip begin of text '\"'
			while(*str.beg != '\"'){
				if(*str.beg == '\\') ++str.beg;//skip '\\'
				++str.beg;//skip this char
			}
			++str.beg;//skip end of text '\"'
			break;
		//char
		case '\'':
			++str.beg;//skip begin of char '\''
			if(*str.beg == '\\') ++str.beg;
			++str.beg;//skip end of char '\''
			break;
		//trans
		case '\\':
			++str.beg;
			++str.beg;
			break;
		default:
			++str.beg;
		}
	}

	temp.length= str.beg- temp.beg;
	pl_vector_push_Tpl_substr(&ret, &temp);
	return ret;//this don't run as usual
}

pl_substr pl_str_trim(pl_substr s){
	while( s.length &&(
		*s.beg==' ' ||
		*s.beg=='\t' ||
		*s.beg=='\n' ||
		*s.beg=='\r')){
			++s.beg;
			--s.length;
	}
	while(s.length &&(
		s.beg[s.length-1]==' ' ||
		s.beg[s.length-1]=='\t' ||
		s.beg[s.length-1]=='\n' ||
		s.beg[s.length-1]=='\r'))
			--s.length;

	return s;
}

int pl_str_is_symbol(pl_substr s, pl_substr bigstr){
	if( s.beg != bigstr.beg && pl_str_is_ablechar(s.beg[-1]) ) 
		return 0;
	if( s.beg+s.length != bigstr.beg+bigstr.length && pl_str_is_ablechar(s.beg[s.length]) ) 
		return 0;
	if( pl_str_get_symbol(s).length != s.length) return 0;
	return 1;
}

pl_vector_Tpl_substr pl_str_cut_symbol(pl_substr s){
	pl_vector_Tpl_substr ret;
	pl_substr substr_temp;
	pl_substr substr_temp_l;
	pl_vector_init_Tpl_substr(&ret);
	substr_temp_l.beg= s.beg;

	while(s.length){
		s= pl_str_jump_text(s);
		substr_temp= pl_str_get_symbol(s);
		if(substr_temp.length){
			if(substr_temp.beg != substr_temp_l.beg){
				substr_temp_l.length= pl_substr_indexof(&substr_temp_l, substr_temp.beg);
				pl_vector_push_Tpl_substr(&ret, &substr_temp_l);
			}
			pl_vector_push_Tpl_substr(&ret, &substr_temp);
			substr_temp_l.beg= substr_temp.beg+ substr_temp.length;
			pl_substr_pophead_n(&s, substr_temp.length);
		}else{
			pl_substr_pophead(&s);
		}
	}
	if(s.beg != substr_temp_l.beg){
		substr_temp_l.length= pl_substr_indexof(&substr_temp_l, s.beg);
		pl_vector_push_Tpl_substr(&ret, &substr_temp_l);
	}
	return ret;
}



#endif//PL_STRHELP