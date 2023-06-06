#pragma once	

typedef struct pending Pending;

int insert_if_not_inside(Pending **, char* );

Pending* remove_from_pending(Pending *, char* );
