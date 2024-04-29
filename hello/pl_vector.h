#ifndef PL_VECTOR
#define PL_VECTOR
//---------------ÓÃ·¨---------------
//#include "pl_vector.h"
//
//PL_VECTOR_DEFALL(int)
//
//int main(){
//	size_t i;
//	pl_vector_Tint vi;
//	pl_vector_init_Tint(&vi);
//
//	pl_vector_push_cp_Tint(&vi, 19);
//	pl_vector_push_cp_Tint(&vi, 29);
//	pl_vector_push_cp_Tint(&vi, 39);
//	pl_vector_erase_Tint(&vi, 1);
//
//	for(i= 0; i < vi.length; i++){
//		printf("vi.data[%d]= %d \n", i, vi.data[i]);
//	}
//
//	pl_vector_free_Tint(&vi);
//
//	system("pause");
//	return 0;
//}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define PL_VECTOR_DEF(type) \
	typedef struct _pl_vector_T##type{  \
		type* data; size_t size; size_t length; \
	} pl_vector_T##type ;

#define PL_VECTOR_INIT_DEF(type)  \
	void pl_vector_init_T##type(pl_vector_T##type* obj){ \
		obj->data= (type*) malloc(sizeof(type)); \
		obj->size= 1; \
		obj->length= 0; \
	}

#define PL_VECTOR_FREE_DEF(type)  \
	void pl_vector_free_T##type(pl_vector_T##type* obj){ \
		free(obj->data); \
	}

#define PL_VECTOR_RESIZE_DEF(type)  \
	void pl_vector_resize_T##type(pl_vector_T##type* obj, size_t newsize){ \
		type* temp; \
		if(newsize <= obj->size) return; \
		temp= (type*) malloc( sizeof(type)*newsize ); \
		obj->size= newsize; \
		memcpy( temp, obj->data, sizeof(type)*(obj->length)); \
		free(obj->data); \
		obj->data= temp; \
	}

#define PL_VECTOR_PUSH_DEF(type) \
	type* pl_vector_push_T##type(pl_vector_T##type* obj, type* n){ \
		if(obj->length+1 >= obj->size) \
			pl_vector_resize_T##type(obj, obj->size<<1 ); \
		obj->data[obj->length]= *n; \
		obj->length++; \
		return &(obj->data[obj->length-1]); \
	}\
	type* pl_vector_push_cp_T##type(pl_vector_T##type* obj, type n){ \
		if(obj->length+1 >= obj->size) \
			pl_vector_resize_T##type(obj, obj->size<<1 ); \
		obj->data[obj->length]= n; \
		obj->length++; \
		return &(obj->data[obj->length-1]); \
	}

#define PL_VECTOR_ERASE_DEF(type) \
	void pl_vector_erase_T##type(pl_vector_T##type* obj, size_t index){ \
		if(index >= obj->length) return; \
		memmove(&obj->data[index], &obj->data[index+1], sizeof(type)*(obj->length-index-1) ); \
		obj->length--; \
		return; \
	}

#define PL_VECTOR_FINDIF_DEF(type) \
	size_t pl_vector_findif_T##type(pl_vector_T##type* obj, type* v,int(*cmp)(type*,type*)){ \
	size_t i; \
		for(i=0;i<obj->length;i++){ \
			if( cmp(&obj->data[i], v) ) \
				return i; \
		} \
		return -1; \
	}

#define PL_VECTOR_INSERT_DEF(type) \
	type* pl_vector_insert_T##type(pl_vector_T##type* obj, type* n, size_t index){ \
		if(obj->length++ >= obj->size) \
			pl_vector_resize_T##type(obj, obj->size<<1 ); \
		memmove(&obj->data[index+1], &obj->data[index], sizeof(type)*(obj->length-index) ); \
		obj->data[index]= *n; \
		return &obj->data[index]; \
	}

#define PL_VECTOR_DEFALL(type) \
	PL_VECTOR_DEF(type) \
	PL_VECTOR_INIT_DEF(type) \
	PL_VECTOR_FREE_DEF(type) \
	PL_VECTOR_RESIZE_DEF(type) \
	PL_VECTOR_PUSH_DEF(type) \
	PL_VECTOR_ERASE_DEF(type) \
	PL_VECTOR_FINDIF_DEF(type) \
	PL_VECTOR_INSERT_DEF(type) \






#endif