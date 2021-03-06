#include "dns.h"

using namespace std;

char * convertName(char * name);
void getHost(int serverPort, string servIP, char* name);

int main (int argc, char* argv[]){
  int timeout = 5;
  int retry = 4;
  int serverPort = 53;
  string servIP;
  char* name;

  
  /* Read in command line arguments */
  
  for(int i = 1; i < argc; i++){
    if(strcmp(argv[i],"-t") == 0){
      timeout = atoi(argv[i+1]);      /* timeout  in seconds*/
    }
    if(strcmp(argv[i],"-r") == 0){
      retry = atoi(argv[i+1]);        /* retries */
    }
    if(strcmp(argv[i],"-p") == 0){
      serverPort = atoi(argv[i+1]);   /* DNS server's UDP port */
    }
    if(argv[i][0] == '@' ){
    	servIP = argv[i];               /* IP address of DNS Server (dotted quad) */
    	servIP.erase(0,1);
    }
    else{
    	name = argv[i];                 /* domain name*/
    }
  }
  	
  for(int i = 0; name[i] != '\0'; i++){
 		cout << name[i];
 	}
 	cout << endl;	
  	
	getHost(serverPort, servIP, name);
 
}

/******************************************************************************/

void getHost(int serverPort, string servIP, char* name){
	unsigned char buf[65536], *reader;
	
	int soc, i;
	
	struct sockaddr_in a;
	struct sockaddr_in destination;
	
	struct HEADER *dnsHeader;
	struct QUESTION *question;
	
	soc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
	destination.sin_family = AF_INET;
	destination.sin_port = htons(serverPort);
	destination.sin_addr.s_addr = inet_addr(servIP.c_str());
	
	srand(time(NULL));
	
	/*Set dnsHeader*/
	dnsHeader = (struct HEADER *)&buf;
	dnsHeader->ID = (rand() % 65536); 	/*2^16*/
	
	dnsHeader->QR = 0;							/*0 for query*/
	dnsHeader->OpCode = 0;					/*Standard query*/ 
	dnsHeader->AA = 0;
	dnsHeader->TC = 0;							/*Not truncated*/
	dnsHeader->RD = 1;							/*Recursion desired*/
	dnsHeader->RA = 0;							
	dnsHeader->Z = 0;
	dnsHeader->RCODE = 0;
	
	dnsHeader->Q_COUNT = htons(1);					/*1 question*/
	dnsHeader->A_COUNT = 0;					/*0 answers*/
	dnsHeader->N_COUNT	= 0;					/*Disregard*/
	dnsHeader->AR_COUNT = 0;					/*Disregard*/
	

	
	name = convertName(name);

	/*place name in array*/
	for(int i = 0; i <= strlen(name); i++){
		if(i == strlen(name)){
			buf[sizeof(struct HEADER)+i] = 0;	
		}
		else{
			buf[sizeof(struct HEADER)+i] = name[i];	
		}
	}
	
	/*Point question to end of header & name*/
	question = (struct QUESTION *)&buf[sizeof(struct HEADER)+strlen(name) + 2];
	

	question->Q_TYPE = 1;
	question->Q_CLASS = 1;

	cout << "Sending packet...\n";
	if(sendto(soc, (char*)buf, sizeof(struct HEADER) + sizeof(struct QUESTION) + (strlen(name)+1),0,(struct sockaddr*)&destination, sizeof(destination))<0){
		cout << "Error Sending";
	}
	cout << "Done.\n";
	

	
	//i = sizeof destination;
	
	cout << "Receiving answer...\n";
	if(recvfrom(soc,(char*)buf, 65535, 0, (struct sockaddr*)&destination, (socklen_t*)&i)<0){
		cout << "Error Receiving";
	}
	cout << "Done\n";
	
	dnsHeader = (struct HEADER *)buf;
	
	int location = sizeof(struct HEADER) + sizeof(struct QUESTION) + (strlen(name)+1);
	reader = &buf[location];
	
	cout <<"The response contains\n";
	cout << ntohs(dnsHeader->Q_COUNT) <<" Questions.\n";	
	cout << ntohs(dnsHeader->A_COUNT) <<" Answers.\n";
	
	unsigned short Type;
	unsigned long int TTL;
	unsigned short DL;
	location = 0;
	char * ptr;
	for(int i = 0; i < ntohs(dnsHeader->A_COUNT); i++){
		location = location+3;
		Type = reader[location];
		//cout << "Type: " << Type << endl;
		if(Type == 5){
			location = location + 5;
			memcpy(&TTL,&reader[location],2);
			TTL = ntohs(TTL);
			//cout << "Time: " << TTL << endl;
			location = location + 2;
			
			memcpy(&DL,&reader[location],2);
			DL = ntohs(DL);
			//cout << "Data Length: " << DL << endl;
			
			location = location +2;
			unsigned char address[DL+strlen(name)];
			unsigned short temp;
			unsigned char length;
			for(int j = 0; j < DL; j ++){
				temp = reader[location];
				if(temp == 192){
					location++;
					temp = reader[location];
					for(int k = ((int) temp-12); k < strlen(name);k++){
						if(name[k] < 47  && name[k] > 0){
							address[j] = '.';
							j++;
						}
						else{
							address[j] = name[k];
							j++;
						}
					}
					 
				}
				else{
					address[j] = reader[location];
					location++;	
				}
			}
			
			cout <<endl;
			cout << "CNAME\t";
			for(int j = 0; address[j] != '\0'; j++){
				cout << address[j];
			}
			
			cout << "\t" << TTL << "\t" << ntohs(dnsHeader->AA)<<endl;
			//cout <<"location: " << location<<endl;
			location = location+3;
		}		
		if(Type == 1){
			
			
			location = location + 3;
			memcpy(&TTL,&reader[location],4);
			TTL = ntohs(TTL);
			
			location = location + 2;
			
			memcpy(&DL,&reader[location],2);
			DL = ntohs(DL);
			//cout << "Data Length: " << DL << endl;
			
			location = location + 2;
			char ip[DL+3];
			vector<unsigned short> temp;
			for(int j = 0; j < DL; j++){
				temp.push_back(reader[location]);
				location++;
			}
			
			cout << "IP\t";
			
			for(int j = 0; j < temp.size(); j++){
				if(j != temp.size()-1){
					cout << temp.at(j) << ".";
				}
				else{
					cout << temp.at(j) << "\t";
				}
			}
			cout << TTL << "\t" << ntohs(dnsHeader->AA)<<endl;
		}
		
		
		cout <<endl;
		
	}
	
}




/**************Funtion to convet name into correct format**********************/
char * convertName(char * name){
	int total = 0;
	int length =0;
	
	vector<int> segment;
	vector<char> hostname;
	
	
	/*Create vectors containing length of segments, and the name without .*/
	for(int i = 0; name[i] != '\0'; i++){
		length++;
		total++;
		
		if(name[i] == '.'){
			segment.push_back(length-1);
			length = 0;
		}
		else{
			hostname.push_back(name[i]);
		}
	}
	
	segment.push_back(length);
	
	//create char * piecing together segment lengths and text
	char *result = new char[total+1];
	
	int k = 0;
	int l = 0;
	
	for(int i = 0; i < segment.size(); i++){
		result[k] = segment.at(i);
		
		for(int j = 1; j <= segment.at(i); j++){
			result[k+j] = hostname.at(l);
			l++;
		}
		k = k + segment.at(i) +1;
	}
	
	return result;
	

}
