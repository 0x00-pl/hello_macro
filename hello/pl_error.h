#ifndef PL_ERROR
#define PL_ERROR

void _pl_global_error_hander_default(const char* e){
	//printf(e);
	//system("pause");
}
typedef void(pl_err_hander)(const char*);
pl_err_hander* _pl_global_error_hander= _pl_global_error_hander_default;
void pl_set_error_hander(pl_err_hander* e){
	_pl_global_error_hander= e;
}
void pl_error(const char* e){
	_pl_global_error_hander(e);
}

typedef pl_err_hander pl_msg_hander;
pl_msg_hander* _pl_global_message_hander= _pl_global_error_hander_default;
void pl_set_message_hander(pl_msg_hander* e){
	_pl_global_error_hander= e;
}
void pl_message(const char* e){
	_pl_global_message_hander(e);
}

#endif