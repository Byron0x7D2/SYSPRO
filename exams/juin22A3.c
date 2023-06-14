mutex m;
cond_t builder, worker;
int builder_count = 0, worker_count = 0, mover_count = 0;

acquire_builder(){
	lock(m);
	while(builder_count + worker_count + mover_count >= 4 && mover_count < 1){
		wait(builder, m);
	}
	builder_count++;
	broadcast(worker);
	unlock(m);
}

release_builder(){
	lock(m);
	builder_count--;
	signal(builder);
	signal(worker);
	unlock(m);
}

acquire_worker(){
	lock(m);
	while((builder_count + worker_count + mover_count >= 4 && mover_count < 1)||(builder_count < 1)){
		wait(worker, m);
	}
	worker_count++;
	unlock(m);

}

release_worker(){
	lock(m);
	worker_count--;
	signal(builder);
	signal(worker);
	unlock(m);
}


acquire_mover(){
	lock(m);
	mover_count++;
	broadcast(builder);
	broadcast(worker);
	unlock(m);
}

release_mover(){
	lock(m);
	mover_count--;
	signal(builder);
	signal(worker);
	unlock(m);
}