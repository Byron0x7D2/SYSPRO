mutex m;
cond Cook, Waiter;
int cook_num = 0, waiter_num = 0, cleaner_num = 0;

// create release and acquire functions prototypes

void cook_acquire(){
	lock(m);
	while(cook_num + waiter_num + cleaner_num >= 4 && cleaner_num < 1){
		wait(Cook, m);
	}
	cook_num++;
	unlock(m);
}

void cook_release(){
	lock(m);
	cook_num--;
	broadcast(Waiter);
	broadcast(Cook);
	unlock(m);
}

void waiter_acquire(){
	lock(m);
	while((cook_num + waiter_num + cleaner_num >= 4 && cleaner_num < 1) || cook_num < 1){
		wait(Waiter, m);
	}
	waiter_num++;
	unlock(m);
}

void waiter_release(){
	lock(m);
	waiter_num--;
	broadcast(Waiter);
	broadcast(Cook);
	unlock(m);

}

void cleaner_acquire(){
	lock(m);
	cleaner_num++;
	unlock(m);
}

void cleaner_release(){
	lock(m);
	cleaner_num--;
	broadcast(Waiter);
	broadcast(Cook);
	unlock(m);
}