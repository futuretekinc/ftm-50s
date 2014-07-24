#include <stdio.h> /* Standard input/output definitions */
#include <string.h> /* String function definitions */
#include <unistd.h> /* UNIX standard function definitions */
#include <fcntl.h> /* File control definitions */
#include <errno.h> /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include "qdecoder.h"

int IOT_USIM(qentry_t *pReq) 
{
		// Parse queries.
		qentry_t *req = qcgireq_parse(NULL, 0);

		FILE *fp;
		char buf[64] = {0, };

		// Get values
		char *value = (char *)req->getstr(req, "cmd", false);
		if (strcmp(value, "state") == 0)
		{
				//FILE *fp;
				//char buf[256] = {0, };

				qcgires_setcontenttype(req, "text/xml");
				fp = popen("echo AT+CPIN? > /dev/ttyS1; sleep 0.1", "r");
				if (fp != NULL)
				{
						pclose(fp);
				}
				fp = popen("/www/cgi-bin/scripts/usim.sh", "r");
				while(fgets(buf, sizeof(buf), fp))
				{
						buf[strlen(buf)-1] = 0;
						
						printf("<data>\n");
						printf("<res>OK</res>\n");
						printf("<text>%s</text>\n", buf);
						printf("</data>");

				}
				pclose(fp);
				return 0;
		} 
	
		if (strcmp(value, "state_status") == 0)
		{
				qcgires_setcontenttype(req, "text/xml");
				fp = popen("echo AT+CREG? > /dev/ttyS1; sleep 0.1", "r");
				if (fp != NULL)
				{
						pclose(fp);
				}
				fp = popen("/www/cgi-bin/scripts/is_usim_status.sh", "r");
				while(fgets(buf, sizeof(buf), fp))
				{
						buf[strlen(buf)-1] = 0;
						
						printf("<data>\n");
						printf("<res>OK</res>\n");
						printf("<text>%s</text>\n", buf);
						printf("</data>");

				}
				pclose(fp);
				return 0;

		}

		if (strcmp(value, "state_nwcause") == 0)
		{
				qcgires_setcontenttype(req, "text/xml");
			//	fp = popen("/www/cgi-bin/scripts/sgact.sh", "r");
				fp = popen("cat /tmp/nwcause", "r");
				while(fgets(buf, sizeof(buf), fp))
				{
						buf[strlen(buf)-1] = 0;
						
						printf("<data>\n");
						printf("<res>OK</res>\n");
						printf("<text>%s</text>\n", buf);
						printf("</data>");

				}
				pclose(fp);
				return 0;

		}


		return 0;
}
