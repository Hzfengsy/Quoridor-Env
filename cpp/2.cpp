#include "submit.h"
#include <queue>
extern int ai_side;
std::string ai_name="Your_ai_name_here_k=0.7";

int locx, locy; 
int d = 0;
struct place{
	int a[5];
	int cnt;
};
struct choice{
	int operate;
	double score;
};
int mp[400];
int myloc,othloc,mywall,othwall,mygoal,othgoal;//location
inline place cango(int,int);
inline place cango2(int);
bool checkwall(int x,int y);

choice solve(int myl,int othl,int myg,int othg,int myw,int othw,int step);

int bfs(int,int);
std:: queue <int> bfsque;
int len[400];

inline bool checkempty(int,int);//none use

void init() {
	/* Your code here */
	for (int f1=0;f1<400;f1++)mp[f1]=0;
	for (int f1=2;f1<=18;f1+=2){
		mp[1*20+f1]=1;
		mp[f1*20+1]=1;
		mp[f1*20+19]=1;
		mp[19*20+f1]=1;
	}
	if (ai_side == 0) {
		locx = 2;
		locy = 10;
		d = 2;
	} else {
		locx = 18;
		locy = 10;
		d = -2;
	}
	myloc=locx*20+locy;
	othloc=(20-locx)*20+locy;
	mywall=10;
	othwall=10;
	mygoal=20-locx;
	othgoal=locx;
}

void GetUpdate(std::pair<int, int> location) {
	/* Your code here */
	if (location.first%2==0&&location.second%2==0){
		othloc=location.first*20+location.second;
	} else {
		mp[location.first*20+location.second]=1;
		othwall--;
	}
}


bool temp = true;

std::pair<int, int> Action() {
	/* Your code here */
	/*bfs(myloc,mygoal);
	place cg=cango(myloc,othloc);
	for (int f1=0;f1<cg.cnt;f1++){
		if (len[cg.a[f1]]<len[myloc]){
			myloc=cg.a[f1];
			f1=5;
		}
	}
	*/
	choice possible=solve(myloc,othloc,mygoal,othgoal,mywall,othwall,2);
	if (possible.operate==0){
		bfs(myloc,mygoal);
		place cg=cango(myloc,othloc);
		for (int f1=0;f1<cg.cnt;f1++){
			if ((len[cg.a[f1]]<len[possible.operate]||possible.operate==0)){
				possible.operate=cg.a[f1];
			}
		}
	}
	locx=possible.operate/20;
	locy=possible.operate%20;
	if ((locx+locy)%2==1){
		mp[possible.operate]=1;
		mywall--;
	} else{
		myloc=possible.operate;
	}
	if (temp) {
		temp = false;
		Debug("GO~\n");
		//locx+=d;
		return std::make_pair(locx, locy);
	} else {
		temp = true;
		Debug("Then I'm back~\n");
		//locx+=d;
		return std::make_pair(locx, locy);
	}
}

inline place cango(int my,int oth){
	place ans;
	ans.cnt=0;
	
	if (my-2*20!=oth){
		if (mp[my-20]==0&&mp[my-20-2]==0){
			ans.a[ans.cnt]=my-2*20;
			ans.cnt++;
		}
	} else if(mp[my-20]==0&&mp[my-20-2]==0){
		if (mp[oth-20]==0&&mp[oth-20-2]==0){
			ans.a[ans.cnt]=oth-2*20;
			ans.cnt++;
		} else{
			if (mp[oth-1]==0&&mp[oth-2*20-1]==0){
				ans.a[ans.cnt]=oth-2;
				ans.cnt++;
			}
			if (mp[oth+1]==0&&mp[oth-2*20+1]==0){
				ans.a[ans.cnt]=oth+2;
				ans.cnt++;
			}
		}
	}
	
	if (my+2*20!=oth){
		if (mp[my+20]==0&&mp[my+20-2]==0){
			ans.a[ans.cnt]=my+2*20;
			ans.cnt++;
		}
	} else if (mp[my+20]==0&&mp[my+20-2]==0){
		if (mp[oth+20]==0&&mp[oth+20-2]==0){
			ans.a[ans.cnt]=oth+2*20;
			ans.cnt++;
		} else{
			if (mp[oth-1]==0&&mp[oth-2*20-1]==0){
				ans.a[ans.cnt]=oth-2;
				ans.cnt++;
			}
			if (mp[oth+1]==0&&mp[oth-2*20+1]==0){
				ans.a[ans.cnt]=oth+2;
				ans.cnt++;
			}
		}
	}
	
	if (my-2!=oth){
		if (mp[my-1]==0&&mp[my-2*20-1]==0){
			ans.a[ans.cnt]=my-2;
			ans.cnt++;
		}
	} else if (mp[my-1]==0&&mp[my-2*20-1]==0){
		if (mp[oth-1]==0&&mp[oth-2*20-1]==0){
			ans.a[ans.cnt]=oth-2;
			ans.cnt++;
		} else{
			if (mp[oth-20]==0&&mp[oth-20-2]==0){
				ans.a[ans.cnt]=oth-2*20;
				ans.cnt++;
			}
			if (mp[oth+20]==0&&mp[oth+20-2]==0){
				ans.a[ans.cnt]=oth+2*20;
				ans.cnt++;
			}
		}
	}
	
	if (my+2!=oth){
		if (mp[my+1]==0&&mp[my-2*20+1]==0){
			ans.a[ans.cnt]=my+2;
			ans.cnt++;
		}
	} else if (mp[my+1]==0&&mp[my-2*20+1]==0){
		if (mp[oth+1]==0&&mp[oth-2*20+1]==0){
			ans.a[ans.cnt]=oth+2;
			ans.cnt++;
		} else{
			if (mp[oth-20]==0&&mp[oth-20-2]==0){
				ans.a[ans.cnt]=oth-2*20;
				ans.cnt++;
			}
			if (mp[oth+20]==0&&mp[oth+20-2]==0){
				ans.a[ans.cnt]=oth+2*20;
				ans.cnt++;
			}
		}
	}
	
	
	return ans;
}

inline place cango2(int my){
	place ans;
	ans.cnt=0;
	
	if (mp[my-20]==0&&mp[my-20-2]==0){
		ans.a[ans.cnt]=my-2*20;
		ans.cnt++;
	}
	if (mp[my+20]==0&&mp[my+20-2]==0){
		ans.a[ans.cnt]=my+2*20;
		ans.cnt++;
	}
	if (mp[my-1]==0&&mp[my-2*20-1]==0){
		ans.a[ans.cnt]=my-2;
		ans.cnt++;
	}
	if (mp[my+1]==0&&mp[my-2*20+1]==0){
		ans.a[ans.cnt]=my+2;
		ans.cnt++;
	}

	return ans;
}




inline bool checkempty(int x,int y){
	if (x<=0||x>=20||y<=0||y>=20){
		return true;
	} else return (mp[x*20+y]==0);
}


int bfs(int my,int mygoal){
	for (int f1=0;f1<20*20;f1++)len[f1]=(1<<30);
	for (int f1=2;f1<=18;f1+=2){
		len[mygoal*20+f1]=0;
		bfsque.push(mygoal*20+f1);
	}
	while (!bfsque.empty()){
		place cg=cango2(bfsque.front());
		for (int f1=0;f1<cg.cnt;f1++){
			if (len[cg.a[f1]]==(1<<30)){
				len[cg.a[f1]]=len[bfsque.front()]+1;
				bfsque.push(cg.a[f1]);
			}
		}
		bfsque.pop();
	}
	return len[my];
}

choice solve(int myl,int othl,int myg,int othg,int myw,int othw,int step){
	choice ans;
	ans.score=-1e10;
	ans.operate=0;
	if (othg==othl/20) return ans;
	place cg=cango(myl,othl);
	int minus;
	bfs(othl,othg);
	minus=len[othl];
	bfs(myl,myg);
	minus-=len[myl];
	if (step<=0){
		for (int f1=0;f1<cg.cnt;f1++){
			if (len[cg.a[f1]]<len[myl]&&minus+len[myl]-len[cg.a[f1]]>ans.score){
				ans.score=minus+len[myl]-len[cg.a[f1]]+mywall*2-othwall*2;
				ans.operate=cg.a[f1];
			}
		}
		return ans;
	}
	bool tf[5];
	for (int f1=0;f1<cg.cnt;f1++){
		tf[f1]=(len[cg.a[f1]]<len[myl]);
	}
	for (int f1=0;f1<cg.cnt;f1++){
		if (tf[f1]){
			choice possible=solve(othl,cg.a[f1],othg,myg,othw,myw,step-1);
			if (-possible.score>ans.score){
				ans.score=-possible.score;
				ans.operate=cg.a[f1];
			}
		}
	}
	for (int f1=2;f1<=17;f1++){
		for (int f2=2;f2<=17;f2++){
			if ((myw>0)&&(f1+f2)%2==1&&checkwall(f1,f2)){
				mp[f1*20+f2]=1;
				int minus2=0;
				bfs(othl,othg);
				minus2+=len[othl];
				bfs(myl,myg);
				minus2-=len[myl];
				if (minus2>=minus&&minus2<(1<<29)&&(len[myl]<(1<<29))){
					choice possible=solve(othl,myl,othg,myg,othw,myw-1,step-1);
					if (-possible.score-.7>ans.score){
						ans.score=-possible.score-.7;
						ans.operate=f1*20+f2;
					}
				}
				mp[f1*20+f2]=0;
			}
		}
	}
	return ans;
}
bool checkwall(int x,int y){
	if (((x^y)&1)==0)return false;
	if ((x&1)==1)
	return (mp[x*20+y]==0&&mp[(x^1)*20+(y^1)]==0&&mp[x*20+y-2]==0&&mp[x*20+y+2]==0);
	else
	return (mp[x*20+y]==0&&mp[(x^1)*20+(y^1)]==0&&mp[(x-2)*20+y]==0&&mp[(x+2)*20+y]==0);
	
}