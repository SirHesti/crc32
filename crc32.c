// -h crc32.c
// -? -d C:\hs/tmp\\  ****
// 05.02.22 HS Komplett überarbeitet und Verzeichnis CRC32 erlaubt
// 06.02.22 HS Linux gleiches verhalten; kann das Perlscript ersetzen!

// meine aenderungen


#include "tools.h"
#include "ctools/c_crc32.h"
#include "ctools/c_md5.h"
#include "VERSION.h"

int singleFile(char* filename);
int singleDir(char* dirname);

int Help()
{
    printf ("%s\n",m_PRG_INFO);
    printf ("-?         This Help\n");
    printf ("-5  build md5 too\n");
    printf ("-o  do not build crc32\n");
    printf ("-h  [File] only one File one CRC and display only the CRC !!\n");
    printf ("-d  [Dir]  crc32 for the complete Dir - if no one specified, then the current dir is taken\n");
    printf ("    [File] only the one CRC; if more then one is given then crc [tab] filename is printed\n");
    return 0;
}

char push;
int md5flag;
int crc32flag;
int minimal;

signed int main(int argc, char *argv[])
{

	if (InitTools(argc , argv, "%v%t%m", I_MAJOR, I_MINOR, I_BUILD, I_BETA, LOG_STDERR | LOG_LOGFILE | LOG_SYSLLOG )) return -1;
    if (argc<2)
    {
        printf("%s\n",m_PRG_INFO);
        lprintf ("No Parameter given.");
        return Help() + 1;
    }

    md5flag = 0;
    crc32flag = 1;
    push = LogType;

    InitARG(argc);
    if (argc==3)
    {
        if (!strcmp(argv[1],"-h"))
        {
            minimal=1;
            if (FileOK(argv[2])) singleFile(argv[2]);
            return 0;
        }
    }
    minimal=0;

    if (aChkARG(".5")) md5flag = 1;
    if (aChkARG(".o")) crc32flag = 0;
    if (aChkARG("?")) return Help();
    if (aChkARGlong("help")) return Help();
    if (aChkARG(".d")) push = LogType; // dummy

    if (arg_GetNext(0,argc) == -1)
    {
        printf("%s\n",m_PRG_INFO);
        lprintf ("No Filename to work.");
        return 0;
    }

    int i,e;
    for ( i=0, e=0;; e++)
    {
        i = arg_GetNext( i+1,argc);
        if (i==-1) break;
        if (FileOK(argv[i]))
        {
            singleFile(argv[i]);
            continue;
        }
        if (DirOK(argv[i]))
        {
            singleDir(argv[i]);
            continue;
        }
    }
    arg_Clean();
    return 0;
}

int singleFile(char* filename)
{
    static char s_log   [PATH_MAX+128];
    static char s_screen[PATH_MAX+128];
    static char BUFF[4096*32];
    static char md5buf[16];
    static char md5string[33];

    FILE *F;

    unsigned int crc32;
    MD5_CTX md5sum;

    crc32 = 0xffffffffL;
    MD5Init(&md5sum);

    int  l;

    F = fopen (filename, "rb");
    if (F == NULL)
    {
        lprintf ("%s can't opend from CRC32File", filename);
        return EXIT_FAILURE;
    }
    for (;;)
    {
        if (feof(F)) break;
        l = fread(BUFF, 1, _countof(BUFF), F);
        if (l>0)
        {
            crc32 = AddCRC ( l, BUFF, crc32);
            MD5Update(&md5sum,(unsigned char*)BUFF,(unsigned int)l);
        }
    }
    fclose (F);
    crc32 = (~crc32) & 0xffffffff;
    MD5Final((unsigned char*)md5buf,&md5sum);
    MD5FinalString(md5buf, md5string);

    if (minimal)
    {
        printf("%08x", crc32);
        return EXIT_SUCCESS;
    }

    memset_ex ( s_log, 0, _countof(s_log));
    memset_ex ( s_screen, 0, _countof(s_screen));
    if ( crc32flag )
    {
        sprintf_ex(s_log, "%08x ", crc32);
        sprintf_ex(s_screen, "%08x\t", crc32);
    }
    if ( md5flag )
    {
        strcat_ex (s_log, md5string);
        strcat_ex (s_log, " ");
        strcat_ex (s_screen, md5string);
        strcat_ex (s_screen, "\t");
    }
    strcat_ex (s_log, filename);
    strcat_ex(s_screen, filename);
    LogType = LOG_LOGFILE | LOG_SYSLLOG;
    lprintf ("%s", s_log);
    printf("%s\n", s_screen);
    LogType = push;
    return EXIT_SUCCESS;
}

int singleDir(char* dirname)
{
    DIR *dp;
    struct dirent *ep;
    struct stat buf;

    char uDir[PATH_MAX];
    char fDirFName[PATH_MAX];

    strcpy_ex(uDir, dirname);
    CorrectPath(uDir);
    //lprintf ("uDir = %s", uDir);
    if (!DirOK( uDir ))
    {
        lprintf ("Error with: %s", uDir);
        return 2;
    }
    dp = opendir (uDir);
    if (dp == NULL)
    {
        lprintf ("OPENDIR failed to %s", uDir);
        return 3;
    }

    for (;;)
    {
        ep = readdir (dp);
        if (!ep) break;
        if (!strcmp(ep->d_name,".")) continue;
        if (!strcmp(ep->d_name,"..")) continue;

        sprintf_ex (fDirFName,"%s%s%s", uDir, DIR_SEP, ep->d_name);

        if (stat (fDirFName ,&buf )==-1) continue;
        if (S_ISREG( buf.st_mode ))
        {
            singleFile (fDirFName);
            continue;
        }
    }
    closedir (dp);
    //lprintf ("uDir = %s", uDir);
    return 0;
}
