#ifndef PL_MACRO
#define PL_MACRO
//用法:
//pl_macro_run(...)
#include "pl_error.h"
#include "pl_vector.h"
#include "pl_strhelp.h"

typedef struct _pl_macro_line{
	pl_substr name;
	size_t arg_num;
	pl_vector_Tpl_substr line;
} pl_macro_line;

int _pl_macro_line_init_cmphelp(pl_substr* a, pl_substr* b){
	return ! pl_str_compare(*a, *b);
}
size_t pl_macro_line_init_help_find_l(pl_substr unit){
	const char* old;
	old= unit.beg;
	unit= pl_str_trim( pl_str_jump_text(unit) );
	while(unit.length){
		if(unit.beg[0]=='\\'){
			return unit.beg- old;
		}
		pl_substr_pophead(&unit);
		unit= pl_str_trim( pl_str_jump_text(unit) );
	}
	return -1;
}
pl_macro_line* pl_macro_line_init(pl_macro_line* obj, pl_substr line){
	size_t i, j;
	size_t fr_temp;
	pl_substr substr_temp;
	pl_substr subline;

	pl_vector_Tpl_substr argname;
	pl_vector_Tpl_substr pre_line;
	 
	subline= pl_str_trim(line);
	// 语法检测
	if(*subline.beg!='#') return 0;
	subline.beg++;subline.length--;//'#'
	subline= pl_str_trim(subline);

	if( pl_str_compare(subline, pl_substr_formstr(&substr_temp, "define")) !=0 ) return 0;
	subline.beg+=6;subline.length-=6;//"define"
	subline= pl_str_trim(subline);
	//test end

	pl_vector_init_Tpl_substr(&argname);
	//preinit arg_num
	obj->arg_num=0;
	
	//init name
	obj->name= pl_str_get_symbol(subline);
	pl_substr_pophead_n(&subline, obj->name.length);
	subline= pl_str_trim(subline);
	
	//if 有参数
	if(pl_substr_head(&subline)== '('){
		//init args   (x, y) => vector
		substr_temp= pl_str_get_parentheses(subline);
		//分割参数
		pl_substr_pophead_n(&subline, substr_temp.length);
		subline= pl_str_trim(subline);
		pl_substr_pophead(&substr_temp); // '('

		pl_vector_free_Tpl_substr(&argname);
		argname= pl_str_cut_with_level(substr_temp);

		for(i= 0; i<argname.length; i++){
			if( !pl_str_is_symbol(argname.data[i], line) ){
				//检测 无效的参数名
				pl_vector_erase_Tpl_substr(&argname, i-- /*获得有效迭代器*/);
			}
		}
		//init arg_num
		obj->arg_num= argname.length;
	}
	//init line with arglist
	pl_vector_init_Tpl_substr(&obj->line);
	pre_line= pl_str_cut_symbol(subline);

	for(i=0; i<pre_line.length; i++){
		pre_line.data[i]= pl_str_trim(pre_line.data[i]);
		substr_temp= pre_line.data[i];
		//remove "\\ \r\n" 
		while((j=pl_macro_line_init_help_find_l(pre_line.data[i]))!= -1){
			substr_temp= pl_substr_substr(&pre_line.data[i], 0, j);

			if(substr_temp.length!=0){
				pl_vector_push_Tpl_substr(&obj->line, &substr_temp);
				fr_temp= pl_vector_findif_Tpl_substr(&argname, &substr_temp, _pl_macro_line_init_cmphelp);
				if(fr_temp!= -1){
					obj->line.data[obj->line.length-1].beg= 0;
					obj->line.data[obj->line.length-1].length= fr_temp;
				}
			}
			substr_temp= pre_line.data[i]= pl_substr_substr(&pre_line.data[i], j+1, 99999);
		}

		if(substr_temp.length!=0){
			pl_vector_push_Tpl_substr(&obj->line, &substr_temp);
			fr_temp= pl_vector_findif_Tpl_substr(&argname, &substr_temp, _pl_macro_line_init_cmphelp);
			if(fr_temp!= -1){
				obj->line.data[obj->line.length-1].beg= 0;
				obj->line.data[obj->line.length-1].length= fr_temp;
			}
		}
	}


		pl_vector_free_Tpl_substr(&argname);
	pl_vector_free_Tpl_substr(&pre_line);
	return obj;
}
void pl_macro_line_free(pl_macro_line* obj){
	pl_vector_free_Tpl_substr(&obj->line);
}

size_t pl_macro_line_replace(const pl_macro_line* obj, pl_vector_Tpl_substr args, pl_vector_Tpl_substr args_old,char* dest,const char* end){
	size_t i;
	char* old;
	old= dest;
	for(i=0; i<obj->line.length; i++){
		if(dest>=end){ pl_error("pl: not enough buffer. 数组越界"); return 0; }
		if(obj->line.data[i].beg){
			pl_substr_tostr(&obj->line.data[i], dest);
			dest+= obj->line.data[i].length;
			if(i+1<obj->line.length && obj->line.data[i+1].beg==0 ){
				if(dest-1>=old && dest[-1]=='#'){
					i++;
					if(dest-2>=old && dest[-2]=='#'){
						// ##
						dest-=2;
						while(dest-1>=old && dest[-1]==' ') --dest;
						if(obj->line.data[i].length < args.length){
							pl_substr_tostr(&args_old.data[obj->line.data[i].length], dest);
							dest+= args_old.data[obj->line.data[i].length].length;
						}
					}else{
						// #
						dest--;
						while(dest-1>=old && dest[-1]==' ') --dest;
						*dest++ ='\"';
						if(obj->line.data[i].length < args.length){
							dest+=pl_substr_tostr_as_str(args_old.data[obj->line.data[i].length], dest, end);
						}
						*dest++= '\"';
					}
				}
			}
		}else{
			if(obj->line.data[i].length < args.length){
				pl_substr_tostr(&args.data[obj->line.data[i].length], dest);
				dest+= args.data[obj->line.data[i].length].length;
			}else{
				--dest;
			}
		}
		*dest++ = ' ';
	}
	return dest- old;
}


PL_VECTOR_DEFALL(pl_macro_line)

typedef struct _pl_macro{
	pl_vector_Tpl_macro_line defs;
}pl_macro;

pl_macro* pl_macro_init(pl_macro* obj){
	pl_vector_init_Tpl_macro_line(&obj->defs);
	return obj;
}


int _pl_macro_run_single_line_findhelp(pl_macro_line* a, pl_macro_line* b /* static_cast from (pl_substr*) */ ){
	return !pl_str_compare(*(pl_substr*)b,a->name);
}

size_t pl_macro_run_single_line(pl_macro* obj, pl_substr source, pl_vector_Tpl_substr unreplace, char* dest, const char* end, size_t malloc_buf_size, int* ex/*default is NULL*/){
	char* dest_old;
	int get_ex;
	size_t symbols_i, i, macro_line_index;
	pl_substr substr_temp, substr_temp2;
	pl_macro_line n_macro_line;
	pl_vector_Tpl_substr symbols, each_args, arg_exps_buff;
	if(pl_macro_line_init(&n_macro_line, source)){
		pl_vector_push_Tpl_macro_line(&obj->defs, &n_macro_line);
		return 0;
	}

	if(ex) *ex=0;
	dest_old= dest;
	pl_vector_init_Tpl_substr(&arg_exps_buff);
	pl_vector_init_Tpl_substr(&each_args);
	symbols= pl_str_cut_symbol(source);

	for(symbols_i=0; symbols_i<symbols.length; symbols_i++){

		macro_line_index= pl_vector_findif_Tpl_macro_line(
			&obj->defs, 
			(pl_macro_line*)&symbols.data[symbols_i],
			_pl_macro_run_single_line_findhelp
			);

		if(macro_line_index == -1){
			// unknow symbol
			substr_temp= pl_str_trim(symbols.data[symbols_i]);
			pl_substr_tostr_safe(&substr_temp, dest, end);
			dest+= substr_temp.length;
			*dest++ = ' ';
			continue;
		}
		if(pl_vector_findif_Tpl_substr(
			&unreplace,
			&obj->defs.data[macro_line_index].name,
			_pl_macro_line_init_cmphelp
			) != -1){
				//black list
				substr_temp= pl_str_trim(symbols.data[symbols_i]);
				pl_substr_tostr_safe(&substr_temp, dest, end);
				dest+= substr_temp.length;
			*dest++ = ' ';
				continue;
		}

		if(ex) *ex=1;
		//if need args
		if(obj->defs.data[macro_line_index].arg_num && symbols.length > symbols_i+1){
			//cut args
			substr_temp= pl_str_trim(symbols.data[++symbols_i]);

			substr_temp.length= source.length-(substr_temp.beg-source.beg);//substr_temp.end = source.end;
			if(pl_substr_head(&substr_temp) == '('){
				pl_substr_pophead(&substr_temp); //'('
				pl_vector_free_Tpl_substr(&each_args);
				each_args= pl_str_cut_with_level(substr_temp);

				//修改symbols_i, 去掉()中的内容
				for(;symbols_i<symbols.length; symbols_i++){
					if(symbols.data[symbols_i].beg+symbols.data[symbols_i].length >
						each_args.data[each_args.length-1].beg+each_args.data[each_args.length-1].length){
							symbols.data[symbols_i]= pl_substr_substr(
								&symbols.data[symbols_i],
								pl_substr_indexof(
									&symbols.data[symbols_i],
									each_args.data[each_args.length-1].beg+
									each_args.data[each_args.length-1].length
									),
								99999);
							symbols.data[symbols_i]= pl_str_trim(symbols.data[symbols_i]);
							if(pl_substr_head(&symbols.data[symbols_i]) != ')') pl_error("这... 理论上应该是 ')' 的.");
							pl_substr_pophead(&symbols.data[symbols_i]);

							//至此成功切割掉了参数部分
							symbols_i--;//修复symbols_i
							break;
					}
				}

				pl_vector_push_Tpl_substr(&unreplace, &obj->defs.data[macro_line_index].name);
				for(i=0; i<each_args.length; i++){
					substr_temp.beg= (char*)malloc(malloc_buf_size);
					substr_temp.length= pl_macro_run_single_line(
						obj, 
						each_args.data[i],
						unreplace,
						(char*)substr_temp.beg,
						substr_temp.beg+malloc_buf_size,
						malloc_buf_size,
						0
						);
					pl_vector_push_Tpl_substr(&arg_exps_buff,  &substr_temp);
				}
				pl_vector_erase_Tpl_substr(&unreplace, unreplace.length-1);
			}
		}


		{
		//prelace
			substr_temp.beg= (char*)malloc(malloc_buf_size);
			substr_temp.length= pl_macro_line_replace(&obj->defs.data[macro_line_index], arg_exps_buff, each_args, (char*)substr_temp.beg, substr_temp.beg+malloc_buf_size);
			
			
				pl_vector_push_Tpl_substr(&unreplace, &obj->defs.data[macro_line_index].name);
				
				substr_temp2.beg= dest;
				substr_temp2.length= pl_macro_run_single_line(obj, substr_temp, unreplace, dest, end, malloc_buf_size, &get_ex);

				while(get_ex!=0){
					//flip to buffer
					substr_temp.length= pl_macro_run_single_line(obj, substr_temp2, unreplace, (char*)substr_temp.beg, substr_temp.beg+malloc_buf_size, malloc_buf_size, &get_ex);
					if(get_ex!=0){
					//flip to dest
					substr_temp2.length= pl_macro_run_single_line(obj, substr_temp, unreplace, dest, end, malloc_buf_size, &get_ex);
					}else{
						substr_temp2.length= substr_temp.length;
						memcpy((char*)substr_temp2.beg, substr_temp.beg, substr_temp.length);
					}
				}
				dest+= substr_temp2.length;
				pl_vector_erase_Tpl_substr(&unreplace, unreplace.length-1);
			//free
			free((char*)substr_temp.beg);
		}
		//free temp vals
		for(i=0; i<arg_exps_buff.length; i++) free((char*)arg_exps_buff.data[i].beg);
		arg_exps_buff.length= 0;

	}
	pl_vector_free_Tpl_substr(&arg_exps_buff);
	//no macro has been find
	return dest-dest_old;
}

size_t pl_macro_run(pl_substr code, char* dest, const char* end, size_t malloc_buf_size){
	//分割单行 #开头以\n分割 其余开头以;分割
	pl_vector_Tpl_substr ssnil;
	pl_macro ssm;
	pl_substr single_line;
	char c_ed;
	const char* old;

	old= dest;
	
	pl_vector_init_Tpl_substr(&ssnil);
	pl_macro_init(&ssm);

	while(code.length){
		code= pl_str_trim(code);
		if(pl_substr_head(&code)=='#'){
			c_ed= '\n';
		}else{
			c_ed= ';';
		}
		single_line= pl_str_trim( pl_str_get_until_char(code, c_ed) );
		code= pl_str_trim( pl_str_jump_until_char(code, c_ed) );

		dest+= pl_macro_run_single_line(&ssm, single_line, ssnil, dest, end, malloc_buf_size, 0);
	}

	return dest-old;
}



#endif //PL_MACRO