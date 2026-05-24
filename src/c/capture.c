/****************************************************************************
*
*    Copyright (C) 2026 John Winans
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Lesser General Public
*    License as published by the Free Software Foundation; either
*    version 2.1 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public
*    License along with this library; if not, write to the Free Software
*    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
*    USA
*
* A proof of concept of a C app to initiate captures on a simple PICO-based logic analyzer.
*
* See:
* https://github.com/johnwinans/2070-S-100-bus-analyzer
* https://github.com/johnwinans/guzmanb_logicanalyzer
*
****************************************************************************/


#include <termios.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>

#include "hexdump.h"

#include "LogicAnalyzer_Structs.h"

// Message format:
//
// Escape bytes by stuffing 0xF0 and then xor the byte to send with 0xF0.
// Probably should escape 0x55, 0xAA and 0xF0
//
// 0x55 0xaa CMD [ DATA ] 0xaa 0x55
//
// CMD 0 = ID (no DATA in message)
// CMD 1 = Capture request
// CMD 2 = Update WiFi settings	(wifi only)
// CMD 3 = Read power status	(wifi only)
// CMD 4 = Restart bootloader
// CMD 5 = start blinking
// CMD 6 = stop blinking
//

static int debug = 0;

/**
*****************************************************************************/
static void usage()
{
	fprintf( stderr, "Usage: capture [-t tty]\n" );
}


/**
*****************************************************************************/
static void setControlLines(int port, int dtr, int rts)
{
	struct termios tty;

    tcgetattr( port, &tty );

    cfmakeraw( &tty ); // Sets raw mode, 8N1, etc.

    tty.c_cflag &= ~CRTSCTS; // Disable hardware flow control
    tty.c_cflag |= CLOCAL;  // Ignore modem control lines

    tcsetattr( port, TCSANOW, &tty );

	ioctl( port, dtr ? TIOCMBIS : TIOCMBIC, TIOCM_DTR );
	ioctl( port, rts ? TIOCMBIS : TIOCMBIC, TIOCM_RTS );
}


/**
* Wait for a character to arrive, read it and return its value.
*
* usec is a timeout value. Max allowed = 999999
*****************************************************************************/
static int readChar( int port, uint32_t usec )
{
    int				i;
	int				stat;
    char			ch = 0;
    fd_set			fds;
	struct timeval	tv = { 0, usec };

    FD_ZERO(&fds);
    FD_SET(port, &fds);

	if ( usec > 0 )
    	stat = select(port+1, &fds, NULL, NULL, &tv);
	else
    	stat = select(port+1, &fds, NULL, NULL, NULL);

	if ( stat == 0 )
		return -2;				// timeout
	else if ( stat < 0 )
		return -1;				// error

    ssize_t rc = read(port, &ch, 1);

    i = ch & 0x0ff;

//#define DEBUG_IO
#ifdef DEBUG_IO
	if ( debug ) {
    	printf("Read %2.2X '%c' status=%zd\r\n", i, ch, rc);
	}
#endif

    if (rc == 0 || rc == -1)
    {
		if ( debug )
        	fprintf(stderr, "EOF\r\n");
        return(-1);
    }

    return(i);
}


/**
* Write the requested count bytes or die trying
***************************************************************************/
static ssize_t safewrite(int fd, void *buf, size_t count)
{
    ssize_t s;
    size_t len = 0;
    char *b = (char*)buf;

	if ( debug ) {
		utilHexdumpBuf( stdout, buf, count );
	}

    while( (len<count) && (s = write(fd, &b[len], count-len)) > 0 )
        len+=s;

    if ( s<=0 )
    {
        perror("write");
        assert(0 && "write() failed");
    }

    return len;
}


char    dump_buf[10000000];

/**
* spew data until we timeout waiting.
***************************************************************************/
void dump_response( int port ) {

	size_t	resid = sizeof(dump_buf);
	char	*pb = dump_buf;

	char	*pdump = dump_buf;
	(void)pdump;	// shut up compiler

	while( resid > 0 ) {
		int rc = readChar( port, 999999 );
		if ( rc < 0 )
			break;

		*pb = rc;
		pb += 1;
		resid -= 1;
#if 0
		if ( debug ) {
			if ( *(pb-1) == '\n' ) {
				printf("%p:\n", pdump);
				utilHexdumpBuf( stdout, pdump, pb-pdump );
				pdump = pb;
			}
		}
#endif
	}

	if ( debug ) {
		printf("Response:\n");
		utilHexdumpBuf( stdout, dump_buf, pb-dump_buf );
	}
}

/**
***************************************************************************/
size_t load_response( int port, char *buf, size_t len, bool dump )
{
	size_t  resid = sizeof(dump_buf);
	char    *pb = buf;

	int rc = 0;
	while( resid > 0 ) {
		rc = readChar( port, 999999 );
		if ( rc == -1 )
			break;			// error
		if ( rc == -2 ) {
			if ( pb-buf > 0 )
				break;			// timeout & I got something
			else
				continue;
		}

		*pb = rc;
		pb += 1;
		resid -= 1;
	}
	if ( debug ) {
		printf("Response: (rc:%d)\n", rc);
		utilHexdumpBuf( stdout, buf, pb-buf );
	}

	return pb-buf;		// actual length read
}


/**
* Request the ID of the LA & dump the reply.
***************************************************************************/
void req_id( int port )
{
	char msg[128];
	msg[0] = 0x55;
	msg[1] = 0xAA;
	msg[2] = 0;		// ID request
	msg[3] = 0xAA;
	msg[4] = 0x55;

	if ( debug ) {
		printf("Sending:\n");
	}

	safewrite( port, msg, 5 );

	if ( debug ) {
		dump_response( port );
	}
	//load_response( port, dump_buf, sizeof(dump_buf), 1 );
}


/**
* Send the given capture_request.
* @bug need to escape the cr!!!
***************************************************************************/
int req_simple( int port, capture_request* cr )
{
	char hdr[] = { 0x55, 0xAA, 0x01 };
	char trl[] = { 0xAA, 0x55 };

	if ( debug ) {
		printf("Sending:%d\n", (int)(sizeof(hdr)+sizeof(*cr)+sizeof(trl)));
	}

	// XXX properly encode the cr data message here!!!

	safewrite( port, hdr, sizeof(hdr) );
	safewrite( port, cr, sizeof(*cr) );
	safewrite( port, trl, sizeof(trl) );

	// read the ACK
	// XXX Hack for now using a timeout
	// XXX Need a better protocol with proper framing.
	if ( load_response( port, dump_buf, sizeof(dump_buf), 1 ) <= 0 )
		return -1;
	//dump_response( port );

	// wait for and read the capture response

	// v6_5 uses 0=1, 1=2 or 2=4 byte sizes 
	uint32_t sample_size = 1 << cr->captureMode;

	size_t response_len = 4+sample_size*(cr->preSamples+cr->postSamples+cr->loopCount);

	int rc = 0;
	while ( rc == 0 ) {
		rc = load_response( port, dump_buf, response_len, 1 );
#if 0
		printf(".");
		fflush(stdout);
#endif
	}

	if ( rc < 4 )
		return -1;	// illegal runt data

	// expect a uint32_t length (little endian) followed by binary sample data
	// the bytes to receive are the length scaled by the cr->captureMode
	uint32_t len = ((uint32_t)dump_buf[0]) | ((uint32_t)dump_buf[1])<<8 | ((uint32_t)dump_buf[2])<<16 | ((uint32_t)dump_buf[3])<<24;

	if ( debug ) {
		printf("Want %d bytes (got %zu)\n", 4+len*sample_size, response_len);
	}

	char *data = dump_buf+4;

	// render each row
	for (int i=0; i < len; ++i) {
		int offset = i * sample_size;
		for (int j=sample_size-1; j>-1; --j) {
			uint32_t val = data[offset+sample_size-j-1];
			for (int k=0; k<8; ++k ) {
				printf("%s%d", (k>0 || j<sample_size-1) ? "," : "", (val&0x01)!=0 );
				val >>= 1;
			}
		}
		printf("\n");
	}

	return 0;
}

/**
***************************************************************************/
int main(int argc, char **argv)
{
	const char 		*tty = "/dev/ttyACM0";
	int				port;
	int				c;

	extern char *optarg;
	while((c = getopt(argc, argv, "dt:")) != -1) {

		switch (c) {
		case 't':
			tty = optarg;
			break;

		case 'd':
			debug = 1;
			break;

		default:
			usage();
			exit(1);
		}
	}


	if ( argc-optind != 6 ) {
		usage();
		exit(1);
	}



    // Trigger definition in the form of "TriggerType:(Edge, Fast or Complex),Channel:(base trigger channel),Value:(binary string indicating each trigger chanel state)".
	// consider changing this to simply: (Edge|Fast|Complex),5,xxxxxx1x1x001
	// capture -t /dev/ttyACM0      1000000 1:SDA,2:SCL 512 1024 0 TriggerType:Edge,Base:5,Value:1
	// capture -i 192.168.0.45:4545 1000000 1,2,3,4     512 1024 0 TriggerType:Complex,Base:5,Value:1101

	// ./capture 10000000 1,2,3,4,5,6,7 512 1024 0 Edge,5,xxxxxx1x1x001


	capture_request	cr = { 0 };

    // Desired sampling frequency.
	cr.frequency = strtol( argv[optind++], NULL, 10 );

	// XXX
    // List of channels to capture (channels separated by comma, can contain a name adding a semicolon after the channel number).
	// map each output column to the associated input chennel
	// for now, make it 1:1
	for ( int i=0; i<32; ++i )
		cr.channels[i] = i;
	cr.channelCount = 16;
	++optind;

    // Number of samples to capture before the trigger.
	cr.preSamples = strtol( argv[optind++], NULL, 10 );

    // Number of samples to capture after the trigger.
	cr.postSamples = strtol( argv[optind++], NULL, 10 );

    // Number of bursts to capture (0 or 1 to disable burst mode).
	cr.loopCount = strtol( argv[optind++], NULL, 10 );






	if ((port = open(tty, O_RDWR | O_NOCTTY | O_NONBLOCK, 0)) < 0) {
		fprintf( stderr, "Can't open tty: %s\n", tty);
		exit(-1);
	}
	setControlLines(port, 1, 1);

	safewrite(port, "\x00", 1);	// hack to un-stick a possibly running capture

	req_id( port );




	// a quick hack that I hope will not include any 0x55 or 0xaa bytes
	cr.triggerType = 0;			// Edge (simple)
	cr.trigger = 0;
	cr.inverted = 1;
	cr.triggerValue = 0x0000;

	cr.measure = 0;

	// this should be determined by the number of channels that are to be sampled
	cr.captureMode = 1;		// 0 = 8bit, 1=16-bit, 2=32-bit

	req_simple( port, &cr );

	close( port );
	exit( 0 );
}
