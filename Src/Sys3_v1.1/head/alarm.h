/*
 * Hardware bits
 */
#define DTR   00002	/* Data Terminal Ready (primary alarm) */
#define R2S   00004	/* Request to Send (clear signal) */
#define RCT   00010	/* Secondary Transmit Data (auxiliary alarm) */
#define RCR   02000	/* Secondary Received Data (primary alarm status) */
#define C2S  020000	/* Clear to Send (auxiliary alarm status) */
#define RCHAR   063	/* The "reset character" */

/*
 * Function codes
 */
#define ALPRI   002000  /* Primary set, and test mask */
#define ALAUX   020000  /* Auxiliary set, and test mask */
#define ALTIME  000377  /* Timeout delay value mask */
#define ALSET   001000  /* Set timer delay */
#define ALRESET 004000  /* Restart watchdog timer */
#define ALCLEAR 010000  /* Clear all alarms */

struct sgtty alsgtty;

#define alrmopn(name)	(alfildes = open(name,2))
#define alrmclo()	close(alfildes)
#define alrm(cmd) \
	(&alsgtty->sg_flags = cmd, stty(alfildes, &alsgtty), &alsgtty->sg_flags)
