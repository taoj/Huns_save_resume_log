#include <stdio.h>

#include <stdlib.h>
#include <libvirt/libvirt.h>
#include <sys/time.h>
#include <unistd.h>

struct timeval save_start,save_end,resume_start,resume_end;
double timeofsave,timeofresume;

int main()
{

	virConnectPtr conn;
	//number of active domain
//	int* activedomain;
	int i;
	conn = virConnectOpen("qemu:///system");
	if (conn == NULL) {
		fprintf(stderr, "Failed to open connection to qemu:///system\n");
		return 1;
	}

	int numofdomain=0;
	
	numofdomain=virConnectNumOfDomains(conn);//get the number of domain

	printf("number of domain is %d\n",numofdomain);

	int* domainID=malloc(sizeof(int)*numofdomain);//get domain id

	numofdomain=virConnectListDomains(conn,domainID,numofdomain);
	
	for(i=0;i<numofdomain;i++)
	{
		printf("domain ID is %d \n",domainID[i]);
		
		}
//	free(domainID);
	
	numofdomain=virConnectNumOfDefinedDomains(conn);

	printf("number of defined domains is %d \n",numofdomain);

//get domain pointer
	virDomainPtr domain_handle;

//get the handle of the first domain

	domain_handle=virDomainLookupByID(conn,domainID[0]);
	
	if(domain_handle==NULL)

	printf("ID error \n");
//path of save file
	char* filename="/var/lib/libvirt/save_domain/save.img";
	
//exe  vmstat
	pid_t p_vm = fork();
	if(p_vm == 0)
	{
		execl("/home/vpathak/Documents/program/vmstat.sh","./vmstat.sh","",(char*)0);
		printf("child \n");
		return;
	
	}

	if(p_vm < 0)
	{
		printf("p_vm error \n");
		return;
	}
//exe iostat
	pid_t p_io = fork();
	if(p_io == 0)
	{
		execl("/home/vpathak/Documents/program/iostat.sh","./iostat.sh","",(char*)0);
		printf("child 2 \n");	
		return;
	}
	if(p_io < 0)
	{
		printf("p_io error \n");
		return;
	}

	gettimeofday(&save_start,NULL);
	if(virDomainSave(domain_handle,filename))
		printf("save failed \n");
	gettimeofday(&save_end,NULL);

	int newID;
	
	gettimeofday(&resume_start,NULL);
	if(newID=virDomainRestore(conn,filename)<0)
	
	printf("restore error \n");
	gettimeofday(&resume_end,NULL);
	
	timeofsave=1000000*(save_end.tv_sec-save_start.tv_sec)+save_end.tv_usec-save_start.tv_usec;

	timeofresume=1000000*(resume_end.tv_sec-resume_start.tv_sec)+resume_end.tv_usec-resume_start.tv_usec;

	printf("save time is %f us\nresume time is %f us\n",timeofsave,timeofresume);
	
	wait(NULL);
	free(domainID);
	virConnectClose(conn);
	return 0;}

