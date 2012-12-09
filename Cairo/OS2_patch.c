
/* This file is to patch the Cairo source code so it will have the OS/2 specific changes */

/* Author: Doodle <doodle@scenergy.dfmk.hu> */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ADD_CAIRO_CALL_TO_FUNCTIONS

int cairoint(char *path)
{
  FILE *hFile, *hTarget;
  char achBuffer[1024], achBuffer2[1024];
  int bMutexPatchDone;
  int bLineReplaced;
  int iPatched;

  sprintf(achBuffer, "%s\\src\\cairoint.h", path);
  hFile = fopen(achBuffer, "rt");
  if (!hFile)
  {
    fprintf(stdout, "INFO: File [%s] could not be opened for patching!\n", achBuffer);
    return 0;
  }

  sprintf(achBuffer, "%s\\src\\$$$_cairoint.h", path);
  hTarget = fopen(achBuffer, "wt");
  if (!hFile)
  {
    fprintf(stdout, "INFO: Tempfile for [%s] could not be opened for patching!\n", achBuffer);
    fclose(hFile);
    return 0;
  }

  bMutexPatchDone = 0;
  iPatched = 0;
  while (!feof(hFile))
  {
    bLineReplaced = 0;
    achBuffer[0] = 0;
    fgets(achBuffer, sizeof(achBuffer), hFile);
    if (achBuffer[0]==0) break;

    if (strstr(achBuffer, "#if defined(__OS2__)") == achBuffer)
      bMutexPatchDone = 1;

    if ((strstr(achBuffer, "#ifndef CAIRO_MUTEX_DECLARE") == achBuffer) &&
	(!bMutexPatchDone))
    {
      // Include the OS/2 mutex defines!
      fprintf(hTarget, "#if defined(__OS2__) && !defined(CAIRO_MUTEX_DECLARE)\n");
      fprintf(hTarget, "\n");
      fprintf(hTarget, "#define INCL_BASE\n");
      fprintf(hTarget, "#define INCL_PM\n");
      fprintf(hTarget, "#include <os2.h>\n");
      fprintf(hTarget, "\n");
      fprintf(hTarget, "# define CAIRO_MUTEX_DECLARE(name) extern HMTX name\n");
      fprintf(hTarget, "# define CAIRO_MUTEX_DECLARE_GLOBAL(name) extern HMTX name\n");
      fprintf(hTarget, "# define CAIRO_MUTEX_LOCK(name) DosRequestMutexSem(name, SEM_INDEFINITE_WAIT)\n");
      fprintf(hTarget, "# define CAIRO_MUTEX_UNLOCK(name) DosReleaseMutexSem(name)\n");
      fprintf(hTarget, "#endif\n");

      bMutexPatchDone = 1;
      iPatched = 1;
    }

    if (!bLineReplaced)
      fputs(achBuffer, hTarget);
  }

  fclose(hFile);
  fclose(hTarget);

  if (!iPatched)
  {
    // Remove temp file, we did not patch the original one
    sprintf(achBuffer, "%s\\src\\$$$_cairoint.h", path);
    remove(achBuffer);
  } else
  {
    // Remove the original file, then rename the temp file!
    fprintf(stdout, "INFO: File [%s\\src\\cairoint.h] has been patched!\n", path);
    sprintf(achBuffer, "%s\\src\\cairoint.h", path);
    sprintf(achBuffer2, "%s\\src\\$$$_cairoint.h", path);
    remove(achBuffer);
    rename(achBuffer2, achBuffer);
  }
  return 1;
}

int cairoh(char *path)
{
  FILE *hFile, *hTarget;
  char achBuffer[1024], achBuffer2[1024];
  int iPatched, bSurfacePatchDone, bLineReplaced;

  sprintf(achBuffer, "%s\\src\\cairo.h", path);
  hFile = fopen(achBuffer, "rt");
  if (!hFile)
  {
    fprintf(stdout, "INFO: File [%s] could not be opened for patching!\n", achBuffer);
    return 0;
  }

  sprintf(achBuffer, "%s\\src\\$$$_cairo.h", path);
  hTarget = fopen(achBuffer, "wt");
  if (!hFile)
  {
    fprintf(stdout, "INFO: Tempfile for [%s] could not be opened for patching!\n", achBuffer);
    fclose(hFile);
    return 0;
  }

  iPatched = 0;
  bSurfacePatchDone = 0;
  while (!feof(hFile))
  {
    bLineReplaced = 0;
    achBuffer[0] = 0;
    fgets(achBuffer, sizeof(achBuffer), hFile);
    if (achBuffer[0]==0) break;

    if (strstr(achBuffer, "CAIRO_SURFACE_TYPE_OS2"))
      bSurfacePatchDone = 1;

    if ((strstr(achBuffer, "} cairo_surface_type_t;")) &&
	(!bSurfacePatchDone))
    {
      // Include the OS/2 surface type!
      fprintf(hTarget, "    , CAIRO_SURFACE_TYPE_OS2\n");
      bSurfacePatchDone = 1;
      iPatched = 1;
    }

    if (!bLineReplaced)
      fputs(achBuffer, hTarget);
  }

  fclose(hFile);
  fclose(hTarget);

  if (!iPatched)
  {
    // Remove temp file, we did not patch the original one
    sprintf(achBuffer, "%s\\src\\$$$_cairo.h", path);
    remove(achBuffer);
  } else
  {
    // Remove the original file, then rename the temp file!
    fprintf(stdout, "INFO: File [%s\\src\\cairo.h] has been patched!\n", path);
    sprintf(achBuffer, "%s\\src\\cairo.h", path);
    sprintf(achBuffer2, "%s\\src\\$$$_cairo.h", path);
    remove(achBuffer);
    rename(achBuffer2, achBuffer);
  }
  return 1;
}

int icimage(char *path)
{
  FILE *hFile, *hTarget;
  char achBuffer[1024], achBuffer2[1024];
  int bLineReplaced;
  int iPatched;

  sprintf(achBuffer, "%s\\pixman\\src\\icimage.c", path);
  hFile = fopen(achBuffer, "rt");
  if (!hFile)
  {
    fprintf(stdout, "INFO: File [%s] could not be opened for patching!\n", achBuffer);
    return 0;
  }

  sprintf(achBuffer, "%s\\pixman\\src\\$$$_icimage.c", path);
  hTarget = fopen(achBuffer, "wt");
  if (!hFile)
  {
    fprintf(stdout, "INFO: Tempfile for [%s] could not be opened for patching!\n", achBuffer);
    fclose(hFile);
    return 0;
  }

  iPatched = 0;
  while (!feof(hFile))
  {
    bLineReplaced = 0;
    achBuffer[0] = 0;
    fgets(achBuffer, sizeof(achBuffer), hFile);
    if (achBuffer[0]==0) break;

    if (strstr(achBuffer, "uint ") != NULL)
    {
      char *pchPos;

      /* This line has to be pathed! */

      while ((pchPos = strstr(achBuffer, "uint ")) != NULL)
      {
        char *pchTemp;
        pchPos += 4;

        pchTemp = pchPos;
        while (*pchTemp) pchTemp++;
        pchTemp+=4;

        while (pchTemp>pchPos)
        {
          pchTemp[0] = pchTemp[-4];
          pchTemp--;
        }
        pchPos[0] = '3';
        pchPos[1] = '2';
        pchPos[2] = '_';
        pchPos[3] = 't';
      }

      fputs(achBuffer, hTarget);
      bLineReplaced = 1;
      iPatched = 1;
    }

    if (!bLineReplaced)
      fputs(achBuffer, hTarget);
  }

  fclose(hFile);
  fclose(hTarget);

  if (!iPatched)
  {
    // Remove temp file, we did not patch the original one
    sprintf(achBuffer, "%s\\pixman\\src\\$$$_icimage.c", path);
    remove(achBuffer);
  } else
  {
    // Remove the original file, then rename the temp file!
    fprintf(stdout, "INFO: File [%s\\pixman\\src\\icimage.c] has been patched!\n", path);
    sprintf(achBuffer, "%s\\pixman\\src\\icimage.c", path);
    sprintf(achBuffer2, "%s\\pixman\\src\\$$$_icimage.c", path);
    remove(achBuffer);
    rename(achBuffer2, achBuffer);
  }
  return 1;
}

int cairo_funcprep(char *path, char *file, char *funcbegin)
{
#ifdef ADD_CAIRO_CALL_TO_FUNCTIONS
  FILE *hFile, *hTarget;
  char achBuffer[1024], achBuffer2[1024];
  int bLineReplaced;
  int iPatched;
  int bFirstLine;

  sprintf(achBuffer, "%s\\src\\%s", path, file);
  hFile = fopen(achBuffer, "rt");
  if (!hFile)
  {
    fprintf(stdout, "INFO: File [%s] could not be opened for patching!\n", achBuffer);
    return 0;
  }

  sprintf(achBuffer, "%s\\src\\$$$_%s", path, file);
  hTarget = fopen(achBuffer, "wt");
  if (!hFile)
  {
    fprintf(stdout, "INFO: Tempfile for [%s] could not be opened for patching!\n", achBuffer);
    fclose(hFile);
    return 0;
  }

  bFirstLine = 1;
  iPatched = 0;
  bLineReplaced = 0;
  sprintf(achBuffer, "");
  while (!feof(hFile))
  {
    // Copy previous line into buffer2
    strcpy(achBuffer2, achBuffer);

    // Read line
    achBuffer[0] = 0;
    fgets(achBuffer, sizeof(achBuffer), hFile);
    if (achBuffer[0]==0) break;

    if ((strstr(achBuffer, funcbegin) == achBuffer) &&
        (strstr(achBuffer, "cairo_public ")!=achBuffer) &&
	(strstr(achBuffer, "(")))
    {
      /* This is a cairo function define! */
      /* Let's see if we have to patch it or not! */

      if ((strstr(achBuffer2, "cairo_public") != achBuffer2) ||
          (strstr(achBuffer, "cairo_call") != achBuffer))
      {
        /* Either the first or second line has to be patched! */

        if (strstr(achBuffer2, "cairo_public") == achBuffer2)
        {
          /* It already has cairo_public, so don't put it there again */
          fprintf(hTarget, "%s", achBuffer2);
        } else
        {
          fprintf(hTarget, "cairo_public %s", achBuffer2);
        }

        if (strstr(achBuffer, "cairo_call") == achBuffer)
        {
          /* It already has cairo_call, so don't put it there again */
          fprintf(hTarget, "%s", achBuffer);
        } else
        {
          fprintf(hTarget, "cairo_call   %s", achBuffer);
        }

        bLineReplaced += 2;
        iPatched = 1;
      }
    }

    if ((strstr(achBuffer, "typedef ") == achBuffer) &&
	(strstr(achBuffer, "(*")) &&
	(strstr(achBuffer, "cairo_call") == NULL)
       )
    {
      char *pchType, *pchRemaining;

      // This is a cairo callback typedef!
      pchType = &(achBuffer[8]);
      pchRemaining = strstr(achBuffer, "(*");
      *(pchRemaining-1) = 0;

      fputs(achBuffer2, hTarget);
      fprintf(hTarget, "typedef %s cairo_call %s", pchType, pchRemaining);

      bLineReplaced += 2;
      iPatched = 1;
    }

    if ((!bLineReplaced) &&
        (!bFirstLine))
      fputs(achBuffer2, hTarget);

    if (bLineReplaced>0)
      bLineReplaced--;
    bFirstLine = 0;
  }

  if (!bLineReplaced)
    fputs(achBuffer2, hTarget);

  fclose(hFile);
  fclose(hTarget);

  if (!iPatched)
  {
    // Remove temp file, we did not patch the original one
    sprintf(achBuffer, "%s\\src\\$$$_%s", path, file);
    remove(achBuffer);
  } else
  {
    // Remove the original file, then rename the temp file!
    fprintf(stdout, "INFO: File [%s\\src\\%s] has been patched!\n", path, file);
    sprintf(achBuffer, "%s\\src\\%s", path, file);
    sprintf(achBuffer2, "%s\\src\\$$$_%s", path, file);
    remove(achBuffer);
    rename(achBuffer2, achBuffer);
  }
#endif
  return 1;
}

int cairo_funcprep2(char *path, char *file, char *funcname)
{
#ifdef ADD_CAIRO_CALL_TO_FUNCTIONS
  FILE *hFile, *hTarget;
  char achBuffer[1024], achBuffer2[1024];
  int bLineReplaced;
  int iPatched;

  sprintf(achBuffer, "%s\\src\\%s", path, file);
  hFile = fopen(achBuffer, "rt");
  if (!hFile)
  {
    fprintf(stdout, "INFO: File [%s] could not be opened for patching!\n", achBuffer);
    return 0;
  }

  sprintf(achBuffer, "%s\\src\\$$$_%s", path, file);
  hTarget = fopen(achBuffer, "wt");
  if (!hFile)
  {
    fprintf(stdout, "INFO: Tempfile for [%s] could not be opened for patching!\n", achBuffer);
    fclose(hFile);
    return 0;
  }

  iPatched = 0;
  bLineReplaced = 0;
  sprintf(achBuffer, "");
  while (!feof(hFile))
  {
    // Read line
    achBuffer[0] = 0;
    fgets(achBuffer, sizeof(achBuffer), hFile);
    if (achBuffer[0]==0) break;

    if (strstr(achBuffer, funcname) == achBuffer)
    {
      fprintf(hTarget, "cairo_call  %s", achBuffer);

      bLineReplaced += 1;
      iPatched = 1;
    }

    if (!bLineReplaced)
      fputs(achBuffer, hTarget);

    if (bLineReplaced>0)
      bLineReplaced--;
  }

  fclose(hFile);
  fclose(hTarget);

  if (!iPatched)
  {
    // Remove temp file, we did not patch the original one
    sprintf(achBuffer, "%s\\src\\$$$_%s", path, file);
    remove(achBuffer);
  } else
  {
    // Remove the original file, then rename the temp file!
    fprintf(stdout, "INFO: File [%s\\src\\%s] has been patched!\n", path, file);
    sprintf(achBuffer, "%s\\src\\%s", path, file);
    sprintf(achBuffer2, "%s\\src\\$$$_%s", path, file);
    remove(achBuffer);
    rename(achBuffer2, achBuffer);
  }
  return 1;
}

int ftconfig(char *path)
{
  FILE *hFile, *hTarget;
  char achBuffer[1024], achBuffer2[1024];
  int iPatched;

  sprintf(achBuffer, "%s\\include\\freetype\\config\\ftconfig.h", path);
  hFile = fopen(achBuffer, "rt");
  if (!hFile)
  {
    fprintf(stdout, "INFO: File [%s] could not be opened for patching!\n", achBuffer);
    return 0;
  }

  sprintf(achBuffer, "%s\\include\\freetype\\config\\$$$_ftconfig.h", path);
  hTarget = fopen(achBuffer, "wt");
  if (!hFile)
  {
    fprintf(stdout, "INFO: Tempfile for [%s] could not be opened for patching!\n", achBuffer);
    fclose(hFile);
    return 0;
  }

  iPatched = 0;
  while (!feof(hFile))
  {
    achBuffer[0] = 0;
    fgets(achBuffer, sizeof(achBuffer), hFile);
    if (achBuffer[0]==0) break;

    if (strstr(achBuffer, "Watcom doesn't provide 64-bit data types"))
    {
      // Found it! Replace it!
      fprintf(hTarget, "  /* this compiler provides the __int64 type */\n");
      fprintf(hTarget, "#define FT_LONG64\n");
      fprintf(hTarget, "#define FT_INT64 __int64\n");

      iPatched = 1;
    }
    else
      fputs(achBuffer, hTarget);
  }

  fclose(hFile);
  fclose(hTarget);

  if (!iPatched)
  {
    // Remove temp file, we did not patch the original one
    sprintf(achBuffer, "%s\\include\\freetype\\config\\$$$_ftconfig.h", path);
    remove(achBuffer);
  } else
  {
    // Remove the original file, then rename the temp file!
    fprintf(stdout, "INFO: File [%s\\include\\freetype\\config\\ftconfig.h] has been patched!\n", path);
    sprintf(achBuffer, "%s\\include\\freetype\\config\\ftconfig.h", path);
    sprintf(achBuffer2, "%s\\include\\freetype\\config\\$$$_ftconfig.h", path);
    remove(achBuffer);
    rename(achBuffer2, achBuffer);
  }
#endif
  return 1;
}

int main(int argc, char *argv[])
{
  if (argc<=2)
  {
    printf("Cairo Source Code Patcher (for cairo-1.2.0)\n");
    printf("Please give me the base directory of\n");
    printf("Cairo and Freetype2 sources\n");
    printf("as two command line parameters!\n");
    return 1;
  }

  // Patch cairoint.h
  if (!cairoint(argv[1]))
    return 1;

  // Patch cairo.h to extend cairo_surface_t with CAIRO_SURFACE_TYPE_OS2
  if (!cairoh(argv[1]))
    return 1;

  // Patch cairo.h
  if (!cairo_funcprep(argv[1], "cairo.h", "cairo_"))
    return 1;

  // Patch cairo-ft.h
  if (!cairo_funcprep(argv[1], "cairo-ft.h", "cairo_"))
    return 1;

  // Patch cairo-ps.h
  if (!cairo_funcprep(argv[1], "cairo-ps.h", "cairo_"))
    return 1;

  // Patch cairo-pdf.h
  if (!cairo_funcprep(argv[1], "cairo-pdf.h", "cairo_"))
    return 1;


  // Patch cairo-svg.h
  if (!cairo_funcprep(argv[1], "cairo-svg.h", "cairo_"))
    return 1;

  // Patch cairo-output-stream.c
  if (!cairo_funcprep2(argv[1], "cairo-output-stream.c", "stdio_write"))
    return 1;

  // Patch cairo-os2.h
  if (!cairo_funcprep(argv[1], "cairo-os2.h", "cairo_"))
    return 1;

  // Patch cairo-os2-surface.c
  if (!cairo_funcprep(argv[1], "cairo-os2-surface.c", "cairo_"))
    return 1;

  /*
  // Patch cairo-scaled-font.c
  if (!cairo_funcprep2(argv[1], "cairo-scaled-font.c", "_cairo_scaled_glyph_destroy"))
    return 1;

  // Patch cairo-svg-surface.c
  if (!cairo_funcprep2(argv[1], "cairo-svg-surface.c", "base64_write_func"))
    return 1;
  */
  /*
  // Patch icimage.c
  if (!icimage(argv[1]))
    return 1;

  // Patch ftconfig.h
  if (!ftconfig(argv[2]))
    return 1;
  */
  return 0;
}

