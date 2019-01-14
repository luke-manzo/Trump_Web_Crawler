
// -----------------------------------
// web.c header file
// -----------------------------------

#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "web.h"

// global variable used by the write_callback function
size_t length = 0;


// ------------------------------------
// Function prototype that is used by the curl library to
// retrieve the webpage (in the response message) and 
// store in the web_t webpage field.
// 
//
// Arguments:	web_t pointer
//
// Return:     	OK on success, FAIL on error
//

size_t write_callback(void *ptr, size_t size, size_t nmemb, char** buffer ) {



	*buffer = (char*)realloc( *buffer, ( length + size*nmemb )*sizeof( char* ) ); 

	strcat( *buffer, (char*) ptr );

	length += (size*nmemb);

	return size * nmemb;

} // end write_callback function

// ------------------------------------
// Function prototype that connects to the web address 
// specified in the web_t url field and retrieves
// the html webpage (using the curl c-library)
// 
//
// Arguments:	web_t pointer
//
// Return:     	OK on success, FAIL on error
//

int get_webpage( web_t* web_struct ) {



	CURL *curl;
  	CURLcode error_code = FAIL;

	length = 0;

  	curl = curl_easy_init();

  	if ( curl ) {

	    	web_struct->webpage = malloc( sizeof( char ) * length );

	    	curl_easy_setopt(curl, CURLOPT_URL, web_struct->url );
	    	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	    	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	    	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &web_struct->webpage );
	 
	    	error_code = curl_easy_perform( curl );

		curl_easy_cleanup( curl );

	}
    
	return ( error_code == OK ) ? OK : FAIL;

} // end get_response function



// ------------------------------------
// Function prototype that parses webpage/URL 
// pointed at by client socket
// 
//
// Arguments:	web_t pointer
//
// Return:     	OK on success, FAIL on error
//
void parse( web_t* web_struct ) {


	int i = 0;//increment for webpage
	int quoteCount;
	char* quote1mark;
	char iMarker;
	int j;//increment for print buffer
	int k =0;//increment through links array
	int urlLength;
	int webpageLength;
	
	//malloc for links
	web_struct->links = malloc(sizeof(char*) * 90);
	for(k;k<90;k++){
		web_struct->links[k] = malloc(sizeof(char) * 300);
	}

	k = 0;

	//setup webpage and lengths
	char* webpage = web_struct->webpage;
	webpageLength = strlen(web_struct->webpage);

	//Null terminate end of webpage
	webpage[webpageLength] = '\0';

	//loop through webpage
	while(i<webpageLength){
		
		//check for out of bounds comparison for an href *do not know if this is neccesary*
		if((i >= webpageLength) || ((i + 1) >= webpageLength) || ((i + 2) >= webpageLength) || ((i + 3) >= webpageLength) ){
			i = 10000000;
		}

		//check if href exists at location
		else if(*webpage == 'h' && *(webpage + 1) == 'r' && *(webpage + 2) == 'e' && *(webpage + 3) == 'f'  ){
            
			webpage++;
			i++;
			//reset quote count to 2 for each new href
			quoteCount = 2;
			
			//loop until we have found to sets of quotes
			while (quoteCount > 0){
			//Search for trump in href as long as quotes not closed
				if((i >= webpageLength) || ((i + 1) >= webpageLength) || ((i + 2) >= webpageLength) || ((i + 3) >= webpageLength) || ((i+4)) >= webpageLength ){
					i = 10000000;
					quoteCount = 0;
				}
				else if(*webpage == '"'){
					if(quoteCount == 2){
						//printf("you found a quote");
						quote1mark = webpage;
						iMarker = i;
						quoteCount=1;
						webpage++;
						i++;
					}
					else if(quoteCount ==1){
						quoteCount= 0;
						webpage++;
						i++;
					}

				}
				else if(((*webpage == 't')  || (*webpage == 'T')) && *(webpage + 1) == 'r' && *(webpage + 2) == 'u' && *(webpage + 3) == 'm' && *(webpage + 4) == 'p' ){

					webpage = quote1mark;
					i = iMarker;
					webpage++;
					i++;
					j =0;
					web_struct->link_cnt += 1;
					char* url = malloc(sizeof(char)* 300);
					url[300] = '\0';
					while((*webpage != '"')){
						url[j] = *webpage;
						webpage++;
						i++;
						j++;

					}
					quoteCount = 0;
					urlLength = j;
					url[j] = '\0';
					j = 0;					
					strcpy(web_struct->links[web_struct->link_cnt -1] , url );
					//printf("Index %d: %s\n ",web_struct->link_cnt,web_struct->links[web_struct->link_cnt]);
					//free(url);		
				}
				else{
					webpage++;
					i++;					
				}
				//quoteCount = 0;	
			}
			 
        }
		
		else{    
		webpage++;
		i++;
		//quoteCount =2;
		}
	}


} // end parse function
