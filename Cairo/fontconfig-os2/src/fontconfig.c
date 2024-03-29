/*
 * This code is (C) Netlabs.org
 * Author: Doodle <doodle@scenergy.dfmk.hu>
 *
 * Version history:
 *   v1.1 (2007/05/16) :
 *     - Implemented some missing functions for Mozilla,
 *       mostly about font sets.
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <math.h> /* for fabs */
#include <float.h> /* for DBL_EPSILON */
#define INCL_DOS
#define INCL_WIN
#define INCL_DOSERRORS
#include <os2.h>
#include <fontconfig/fontconfig.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#ifndef FC_CACHE_VERSION_STRING
# define FC_CACHE_VERSION_STRING "v1.1_with_GCC"
#endif

#ifdef FC_EXPORT_FUNCTIONS
# define fcExport __declspec(dllexport)
#else
# define fcExport
#endif

#define MAX_FONTLISTSIZE 4096

/* structure for the font cache in OS2.INI  */
typedef struct FontDescriptionCache_s
{
  char achFileName[CCHMAXPATH];
  struct stat FileStatus;
  char achFamilyName[128];
  char achStyleName[128];
  long lFontIndex;

  struct FontDescriptionCache_s *pNext;
} FontDescriptionCache_t, *FontDescriptionCache_p;

struct _FcPattern
{
    char *family;
    int slant;
    int weight;
    double pixelsize;
    int spacing;
    FcBool hinting;
    FcBool antialias;
    FcBool embolden;
    FcBool verticallayout;
    int hintstyle;
    FcBool autohint;
    int rgba;
    double size;
    char *style;

    FontDescriptionCache_p pFontDesc;
};


static FT_Library hFtLib;
static FontDescriptionCache_p pFontDescriptionCacheHead;
static FontDescriptionCache_p pFontDescriptionCacheLast;

fcExport void FcFini()
{
  FontDescriptionCache_p pToDelete;

  if (hFtLib) {
    /* Uninitialize FreeType */
    FT_Done_FreeType(hFtLib);
    hFtLib = NULL; /* ensure that we won't do this again */
  }

  /* Destroy Font Description Cache */
  while (pFontDescriptionCacheHead)
  {
    pToDelete = pFontDescriptionCacheHead;
    pFontDescriptionCacheHead = pFontDescriptionCacheHead->pNext;
    free(pToDelete);
  }
}

static void ConstructINIKeyName(char *pchDestinationBuffer, unsigned int uiDestinationBufferSize,
                                char *pchFontName, long lFaceIndex)
{
  snprintf(pchDestinationBuffer, uiDestinationBufferSize,
           "%s%04ld", pchFontName, lFaceIndex);
}

static int CreateCache(FontDescriptionCache_p pFontCache, char *pchFontName,
                       char *pchFontFileName, long lFaceIndex)
{
  FT_Face ftface;
  int rc;
  ULONG ulSize;

  if (FT_New_Face(hFtLib, pchFontFileName, lFaceIndex, &ftface))
  {
    /* Could not load font. */
    return 0;
  }

  strncpy(pFontCache->achFileName,
          pchFontFileName,
          sizeof(pFontCache->achFileName));
  if (stat(pchFontFileName, &(pFontCache->FileStatus))==-1)
  {
    /* Could not get status info, skip this font! */
    return 0;
  }

  if (ftface->family_name)
    strncpy(pFontCache->achFamilyName,
            ftface->family_name,
            sizeof(pFontCache->achFamilyName));
  if (ftface->style_name)
    strncpy(pFontCache->achStyleName,
            ftface->style_name,
            sizeof(pFontCache->achStyleName));
  pFontCache->lFontIndex = lFaceIndex;

  FT_Done_Face(ftface);

  /* construct actual INI key from font name and number */
  char achKeyName[128];
  ConstructINIKeyName(achKeyName, sizeof(achKeyName),
                      pchFontName, lFaceIndex);
  ulSize = sizeof(FontDescriptionCache_t);
  /* Store font cache in INI file */
  rc = PrfWriteProfileData(HINI_USER, "PM_Fonts_FontConfig_Cache_"FC_CACHE_VERSION_STRING,
                           achKeyName, pFontCache, ulSize);

  return rc;
}

static void CacheFontDescription(char *pchFontName, char *pchFontFileName)
{
  int rc;
  FontDescriptionCache_t FontDesc;
  FontDescriptionCache_p pNewFontCacheEntry;
  ULONG ulSize;
  FT_Face ftface;
  FT_Open_Args ftopenargs;
  long lNumFacesInFile;
  long lCurFace;
  int iLen = strlen(pchFontFileName);

  /* Modify filename if needed */
  if (stricmp(pchFontFileName + (iLen-4), ".OFM") == 0)
  {
    pchFontFileName[iLen-3] = 'P';
    pchFontFileName[iLen-1] = 'B';
  }

  /* Query the number of font faces contained in this font file */
  /* Documentation for FT_Open_Face() says that this is the way to */
  /* quickly query the number of supported font faces of a file. */
  ftopenargs.flags = FT_OPEN_PATHNAME;
  ftopenargs.pathname = pchFontFileName;
  ftopenargs.num_params = 0;
  ftopenargs.params = NULL;
  if (FT_Open_Face(hFtLib, &ftopenargs, -1, &ftface))
  {
    /* Could not load font. */
    return;
  }
  lNumFacesInFile = ftface->num_faces;
  FT_Done_Face(ftface);

  /* Now go through all the faces of this font, and check if we have */
  /* a cache entry for all of them in INI file */
  for (lCurFace=0; lCurFace<lNumFacesInFile; lCurFace++)
  {
    /* Construct key name for font file + face index pair */
    char achKeyName[128];
    ConstructINIKeyName(achKeyName, sizeof(achKeyName),
                        pchFontName, lCurFace);

    /* Try to read back the font cache for this pair from the INI file */
    memset(&FontDesc, 0, sizeof(FontDesc));
    ulSize = sizeof(FontDesc);
    rc = PrfQueryProfileData(HINI_USER, "PM_Fonts_FontConfig_Cache_"FC_CACHE_VERSION_STRING,
                             achKeyName,
                             &FontDesc,  &ulSize);
    if ((ulSize!=sizeof(FontDesc)) || (!rc))
    {
      /* Hm, there is no cache for this file, try to create it! */
      if (!CreateCache(&FontDesc, pchFontName, pchFontFileName, lCurFace))
        continue;
    } else
    {
      struct stat statBuf;

      /* There is cache for this file, check if it's up to date! */
      if (stat(pchFontFileName, &statBuf)==-1)
      {
        /* Could not get status info, skip this font! */
        continue;
      }
      if ((statBuf.st_size != FontDesc.FileStatus.st_size) ||
          (statBuf.st_mtime != FontDesc.FileStatus.st_mtime))
      {
        /* The cache is not up to date, so recreate it! */
        if (!CreateCache(&FontDesc, pchFontName, pchFontFileName, lCurFace))
          continue;
      }
    }

    /* Link this font to the list of available fonts */
    pNewFontCacheEntry = (FontDescriptionCache_p) malloc(sizeof(FontDescriptionCache_t));
    if (!pNewFontCacheEntry)
      return;

    memcpy(pNewFontCacheEntry, &FontDesc, sizeof(FontDesc));
    pNewFontCacheEntry->pNext = NULL;
    if (pFontDescriptionCacheLast)
    {
      pFontDescriptionCacheLast->pNext = pNewFontCacheEntry;
      pFontDescriptionCacheLast = pNewFontCacheEntry;
    } else
    {
      pFontDescriptionCacheLast = pFontDescriptionCacheHead = pNewFontCacheEntry;
    }
  }
}

// we need to do case insensitive comparison a lot
char *stristr(const char *str1, const char *str2)
{
  char *upper1, *upper2, *retval;

  // as strupr does in place modification, copy the strings first
  upper1 = strdup(str1);
  upper2 = strdup(str2);
  strupr(upper1);
  strupr(upper2);

  // now call the original strstr on the uppercase strings
  retval = strstr(upper1, upper2);

  // and clean up before returning
  free(upper1);
  free(upper2);
  return retval;
}

fcExport FcBool FcInit()
{
  ULONG ulBootDrive;
  char chBootDrive;
  char achFontNameList[MAX_FONTLISTSIZE];
  char *pchCurrentFont;
  char achFontFileName[CCHMAXPATH];
  char achAbsFontFileName[CCHMAXPATH];

  if (FT_Init_FreeType(&hFtLib))
  {
    /* Could not initialize FreeType */
    return FcFalse;
  }

  /* Go through all the available/installed fonts and
   * make sure we have an up-to-date description cache
   * for all of them */
  pFontDescriptionCacheHead = NULL;
  pFontDescriptionCacheLast = NULL;

  DosQuerySysInfo(QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrive, sizeof(ULONG));
  chBootDrive = (char)( ulBootDrive + '@' );

  memset(achFontNameList, 0, MAX_FONTLISTSIZE);
  PrfQueryProfileString(HINI_USER, "PM_Fonts", NULL, NULL, achFontNameList, MAX_FONTLISTSIZE);

  pchCurrentFont = &(achFontNameList[0]);
  while (pchCurrentFont[0])
  {
    PrfQueryProfileString(HINI_USER, "PM_Fonts", pchCurrentFont, "", achFontFileName, CCHMAXPATH);

    if (achFontFileName[0] == '\\' )
    {
      achAbsFontFileName[0] = chBootDrive;
      achAbsFontFileName[1] = ':';
      achAbsFontFileName[2] = 0;
      strcat(achAbsFontFileName, achFontFileName);
    } else
    {
      strcpy(achAbsFontFileName, achFontFileName);
    }

    CacheFontDescription(pchCurrentFont, achAbsFontFileName);

    pchCurrentFont += strlen(pchCurrentFont)+1;
  }

  return FcTrue;
}

fcExport FcPattern *FcPatternCreate (void)
{
  FcPattern *pResult = malloc(sizeof(FcPattern));
  if (pResult)
  {
    memset(pResult, 0, sizeof(FcPattern));
  }
  pResult->hinting = FcTrue; // default to hinting on
  pResult->antialias = FcTrue; // default to antialias on
  pResult->embolden = FcFalse; // no emboldening by default
  pResult->verticallayout = FcFalse; // horizontal layout by default
  pResult->autohint = FcFalse; // off by default as recommended in fontconfig.h
  /* set the strings to NULL for easy testing */
  pResult->family = NULL;
  pResult->style = NULL;
  return pResult;
}

fcExport void FcPatternDestroy (FcPattern *p)
{
  if (p->family)
    free(p->family);
  if (p->style)
    free(p->style);
  free(p);
}

fcExport FcBool FcConfigSubstitute (FcConfig	*config,
                                    FcPattern	*p,
                                    FcMatchKind	kind)
{
  // STUB
  return FcTrue;
}


fcExport void FcDefaultSubstitute (FcPattern *pattern)
{
  if (pattern)
  {
    if (pattern->weight==0)
      pattern->weight = FC_WEIGHT_MEDIUM;
    if (pattern->slant==0)
      pattern->slant = FC_SLANT_ROMAN;
    if (pattern->spacing==0)
      pattern->spacing = FC_PROPORTIONAL;
    if (pattern->pixelsize==0)
      pattern->pixelsize = 12.0 * 1.0 * 75.0 / 72.0; // font size * scale * dpi / 72.0;
    if (pattern->size==0)
      pattern->size = 12.0;
    /* don't mess with the other settings, especially not with the boolean ones */
  }
}

fcExport FcResult FcPatternGetInteger (const FcPattern *p, const char *object, int n, int *i)
{
  if (strcmp(object, FC_INDEX)==0)
  {
    *i = p->pFontDesc->lFontIndex;
    return FcResultMatch;
  }
  if (strcmp(object, FC_SLANT)==0)
  {
    *i = p->slant;
    return FcResultMatch;
  }
  if (strcmp(object, FC_WEIGHT)==0)
  {
    *i = p->weight;
    return FcResultMatch;
  }
  if (strcmp(object, FC_HINT_STYLE)==0)
  {
    *i = p->hintstyle;
    return FcResultMatch;
  }
  if (strcmp(object, FC_RGBA)==0)
  {
    *i = p->rgba;
    return FcResultMatch;
  }
  return FcResultNoMatch;
}

fcExport FcResult FcPatternGetString (const FcPattern *p, const char *object, int n, FcChar8 ** s)
{
  if (strcmp(object, FC_FILE)==0)
  {
    *s = p->pFontDesc->achFileName;
    return FcResultMatch;
  }

  if (strcmp(object, FC_FAMILY)==0)
  {
    if (p->family)
    {
      *s = p->family;
      return FcResultMatch;
    } else
    if (p->pFontDesc)
    {
      *s = p->pFontDesc->achFamilyName;
      return FcResultMatch;
    }
  }

  if (strcmp(object, FC_STYLE)==0)
  {
    if (p->style)
    {
      *s = p->style;
      return FcResultMatch;
    } else
    if (p->pFontDesc)
    {
      *s = p->pFontDesc->achStyleName;
      return FcResultMatch;
    }
  }

  return FcResultNoMatch;
}

fcExport FcResult FcPatternGetBool (const FcPattern *p, const char *object, int n, FcBool *b)
{
  if (strcmp(object, FC_HINTING)==0)
  {
    *b = p->hinting;
    return FcResultMatch;
  }
  if (strcmp(object, FC_ANTIALIAS)==0)
  {
    *b = p->antialias;
    return FcResultMatch;
  }
  if (strcmp(object, FC_EMBOLDEN)==0)
  {
    *b = p->embolden;
    return FcResultMatch;
  }
  if (strcmp(object, FC_VERTICAL_LAYOUT)==0)
  {
    *b = p->verticallayout;
    return FcResultMatch;
  }
  if (strcmp(object, FC_AUTOHINT)==0)
  {
    *b = p->autohint;
    return FcResultMatch;
  }
  return FcResultNoMatch;
}

fcExport FcResult FcPatternGet (const FcPattern *p, const char *object, int id, FcValue *v)
{
  if (strcmp(object, FC_ANTIALIAS)==0)
  {
    v->type = FcTypeBool;
    v->u.b  = FcTrue;
    return FcResultMatch;
  }
  return FcResultNoMatch;
}


fcExport FcBool FcPatternAddInteger (FcPattern *p, const char *object, int i)
{
  if (strcmp(object, FC_INDEX)==0)
  {
    p->pFontDesc->lFontIndex = i;
    return FcTrue;
  }
  if (strcmp(object, FC_SLANT)==0)
  {
    p->slant = i;
    return FcTrue;
  }
  if (strcmp(object, FC_WEIGHT)==0)
  {
    p->weight = i;
    return FcTrue;
  }
  if (strcmp(object, FC_HINT_STYLE)==0)
  {
    p->hintstyle = i;
    return FcTrue;
  }
  if (strcmp(object, FC_RGBA)==0)
  {
    p->rgba = i;
    return FcTrue;
  }

  return FcFalse;
}

fcExport FcBool FcPatternAddDouble(FcPattern *p, const char *object, double d)
{
  if (strcmp(object, FC_PIXEL_SIZE)==0)
  {
    p->pixelsize = d;
    return FcTrue;
  }
  if (strcmp(object, FC_SIZE)==0)
  {
    p->size = d;
    return FcTrue;
  }

  return FcFalse;
}

fcExport FcResult FcPatternGetDouble(const FcPattern *p, const char *object, int id, double *d)
{
  if (strcmp(object, FC_PIXEL_SIZE)==0)
  {
    *d = p->pixelsize;
    return FcResultMatch;
  }
  if (strcmp(object, FC_SIZE)==0)
  {
    *d = p->size;
    return FcResultMatch;
  }

  return FcResultNoMatch;
}

fcExport FcBool FcPatternAddString (FcPattern *p, const char *object, const FcChar8 *s)
{
  if (strcmp(object, FC_FAMILY)==0)
  {
    if (p->family)
    {
      free(p->family); p->family = NULL;
    }
    p->family = strdup(s);
    return FcTrue;
  }

  if (strcmp(object, FC_STYLE)==0)
  {
    if (p->style)
    {
      free(p->style); p->style = NULL;
    }
    p->style = strdup(s);
    return FcTrue;
  }

  return FcFalse;
}

fcExport FcBool FcPatternAddBool (FcPattern *p, const char *object, FcBool b)
{
  if (strcmp(object, FC_HINTING)==0)
  {
    p->hinting = b;
    return FcTrue;
  }
  if (strcmp(object, FC_ANTIALIAS)==0)
  {
    p->antialias = b;
    return FcTrue;
  }
  if (strcmp(object, FC_EMBOLDEN)==0)
  {
    p->embolden = b;
    return FcTrue;
  }
  if (strcmp(object, FC_VERTICAL_LAYOUT)==0)
  {
    p->verticallayout = b;
    return FcTrue;
  }
  if (strcmp(object, FC_AUTOHINT)==0)
  {
    p->autohint = b;
    return FcTrue;
  }

  return FcFalse;
}

#define DEFAULT_SERIF_FONT          "Times New Roman"
#define DEFAULT_SANSSERIF_FONT      "Helvetica"
#define DEFAULT_MONOSPACED_FONT     "Courier"

fcExport FcPattern *FcFontMatch (FcConfig	*config,
                                 FcPattern	*p,
                                 FcResult	*result)
{
  FontDescriptionCache_p pFont, pBestMatch;
  int iBestMatchScore;
  int bWeightOk;
  int bSlantOk;

  pFont = pFontDescriptionCacheHead;
  pBestMatch = NULL;
  iBestMatchScore = -1;

//#define MATCH_DEBUG
#ifdef MATCH_DEBUG
  // print the list of all fonts for debugging
  printf("\nfull font list\n");
  while (pFont)
  {
    printf("  %s,%s\n", pFont->achFamilyName, pFont->achStyleName);
    pFont = pFont->pNext;
  }
  // reset to first font
  pFont = pFontDescriptionCacheHead;

  // print input pattern to match
  printf("input pattern\n  %s,%d,%d,%f\n",
         p->family, p->weight, p->slant, p->pixelsize);
#endif

  // first try to match the font using an exact match of the family name
  while (pFont)
  {
    if ((p->family) && (stricmp(pFont->achFamilyName, p->family)==0))
    {
      // Family found, calculate score for it!
      if ( p->weight > FC_WEIGHT_MEDIUM )
      {
        // Looking for a BOLD font
        bWeightOk = (stristr(pFont->achStyleName, "BOLD")!=NULL);
      } else
      {
        // Looking for a non-bold (normal) font
        bWeightOk = (stristr(pFont->achStyleName, "BOLD")==NULL);
      }

      if ( p->slant > FC_SLANT_ROMAN )
      {
        // Looking for an ITALIC font
        bSlantOk = (stristr(pFont->achStyleName, "ITALIC")!=NULL);
      } else if ( p->slant > FC_SLANT_OBLIQUE )
      {
        // Looking for an OBLIQUE font
        bSlantOk = (stristr(pFont->achStyleName, "OBLIQUE")!=NULL);
      } else
      {
        // Looking for a non-italic font
        bSlantOk = (stristr(pFont->achStyleName, "ITALIC")==NULL &&
                    stristr(pFont->achStyleName, "OBLIQUE")==NULL);
      }

      // Check if this score is better than the previous best one
      if (iBestMatchScore < bWeightOk*2 + bSlantOk)
      {
        pBestMatch = pFont;
        iBestMatchScore = bWeightOk*2 + bSlantOk;

        // Check if it's a perfect match!
        if ((bWeightOk) && (bSlantOk))
        {
          // Found an exact match!
          break;
        }
      }
    }
    pFont = pFont->pNext;
  }
  // Use the one if we've found something
  if (pBestMatch)
    pFont = pBestMatch;

  // Did not find a good one by family name match, search now with
  // default font families! This includes the OS/2 typical fonts of
  // Tms Rmn and Helv as well as Swiss
  if (!pFont)
  {
    // 64 seems to be the max font name length on OS/2 already, add some margin
    char achKey[128] = "";
    char *achKeySpace = NULL;

    if ( p->spacing == FC_MONO || ((p->family) && (stricmp("MONOSPACE", p->family)==0)))
    {
      strncpy(achKey, DEFAULT_MONOSPACED_FONT, sizeof(achKey));
    }
    else if (
        (p->family) &&
        ((stricmp( p->family, "SWISS" ) == 0 ) ||
         (stricmp( p->family, "HELV" ) == 0 ) ||
         (stricmp( p->family, "SANS-SERIF" ) == 0 ) ||
         (stricmp( p->family, "SANS" ) == 0 )
        )
       )
    {
      strncpy(achKey, DEFAULT_SANSSERIF_FONT, sizeof(achKey));
    }
    else if (
        (p->family) &&
        ((stricmp( p->family, "SERIF" ) == 0 ) ||
         (stricmp( p->family, "TMS RMN" ) == 0 ) ||
         (stricmp( p->family, DEFAULT_SERIF_FONT ) == 0 )
        )
       )
    {
      // this of course includes the case of "Tms Rmn"
      strncpy(achKey, DEFAULT_SERIF_FONT, sizeof(achKey));
      // we want to match Times New Roman which has an additional trailing
      // space in the name...
      achKeySpace = strdup(achKey);
      strcat(achKeySpace, " ");
    }

    pFont = pFontDescriptionCacheHead;
    pBestMatch = NULL;
    iBestMatchScore = -1;
    // only search font list, if we set a key to search for
    while (achKey && achKey[0] && pFont)
    {
      if (stricmp(pFont->achFamilyName, achKey) == 0 ||
          (achKeySpace && stricmp(pFont->achFamilyName, achKeySpace) == 0)
         )
      {
        // Family found, calculate score for it!
        if ( p->weight > FC_WEIGHT_MEDIUM )
        {
          // Looking for a BOLD font
          bWeightOk = (stristr(pFont->achStyleName, "BOLD")!=NULL);
        } else
        {
          // Looking for a non-bold (normal) font
          bWeightOk = (stristr(pFont->achStyleName, "BOLD")==NULL);
        }

        if ( p->slant > FC_SLANT_ROMAN )
        {
          // Looking for an ITALIC font
          bSlantOk = (stristr(pFont->achStyleName, "ITALIC")!=NULL);
        } else if ( p->slant > FC_SLANT_OBLIQUE )
        {
          // Looking for an OBLIQUE font
          bSlantOk = (stristr(pFont->achStyleName, "OBLIQUE")!=NULL);
        } else
        {
          // Looking for a non-italic font
          bSlantOk = (stristr(pFont->achStyleName, "ITALIC")==NULL &&
                      stristr(pFont->achStyleName, "OBLIQUE")==NULL);
        }

        // Check if this score is better than the previous best one
        if (iBestMatchScore < bWeightOk*2 + bSlantOk)
        {
          pBestMatch = pFont;
          iBestMatchScore = bWeightOk*2 + bSlantOk;

          // Check if it's a perfect match!
          if ((bWeightOk) && (bSlantOk))
          {
            // Fount an exact match!
            break;
          }
        }
      }
      pFont = pFont->pNext;
    }
    if (achKeySpace)
      free(achKeySpace);
  }
  // Use the one if we've found something
  if (pBestMatch)
    pFont = pBestMatch;

  // again, if an exact match was not found, now try to find the family
  // name by substring search
  if (!pBestMatch)
  {
    pFont = pFontDescriptionCacheHead;
    pBestMatch = NULL;
    iBestMatchScore = -1;
    while (pFont)
    {
      if ((p->family) && (stristr(pFont->achFamilyName, p->family) != NULL))
      {
        // Family found, calculate score for it!
        if ( p->weight > FC_WEIGHT_MEDIUM )
        {
          // Looking for a BOLD font
          bWeightOk = (stristr(pFont->achStyleName, "BOLD")!=NULL);
        } else
        {
          // Looking for a non-bold (normal) font
          bWeightOk = (stristr(pFont->achStyleName, "BOLD")==NULL);
        }

        if ( p->slant > FC_SLANT_ROMAN )
        {
          // Looking for an ITALIC font
          bSlantOk = (stristr(pFont->achStyleName, "ITALIC")!=NULL);
        } else if ( p->slant > FC_SLANT_OBLIQUE )
        {
          // Looking for an OBLIQUE font
          bSlantOk = (stristr(pFont->achStyleName, "OBLIQUE")!=NULL);
        } else
        {
          // Looking for a non-italic font
          bSlantOk = (stristr(pFont->achStyleName, "ITALIC")==NULL &&
                      stristr(pFont->achStyleName, "OBLIQUE")==NULL);
        }

        // Check if this score is better than the previous best one
        if (iBestMatchScore < bWeightOk*2 + bSlantOk)
        {
          pBestMatch = pFont;
          iBestMatchScore = bWeightOk*2 + bSlantOk;

          // Check if it's a perfect match!
          if ((bWeightOk) && (bSlantOk))
          {
            // Found an exact match!
            break;
          }
        }
      }
      pFont = pFont->pNext;
    }
  }
  // Use the one if we've found something
  if (pBestMatch)
    pFont = pBestMatch;

  if (pFont)
  {
#ifdef MATCH_DEBUG
    // print the font representing the best match
    printf("best match\n  %s,%s\n", pFont->achFamilyName, pFont->achStyleName);
#endif

    // If a font is found, then return with it!
    FcPattern *pResult = FcPatternCreate();
    if (pResult)
    {
      // in the output pattern set the three properties we use to select
      if (pFont->achFamilyName)
        pResult->family = strdup(pFont->achFamilyName);

      if (stristr(pResult->family, "BOLD")!=NULL)
        pResult->weight = FC_WEIGHT_BOLD;
      else
        pResult->weight = FC_WEIGHT_REGULAR;

      if (stristr(pResult->family, "ITALIC")!=NULL)
        pResult->slant = FC_SLANT_ITALIC;
      else if (stristr(pResult->family, "OBLIQUE")!=NULL)
        pResult->slant = FC_SLANT_OBLIQUE;
      else
        pResult->slant = FC_SLANT_ROMAN;

      // If we found the font name of generic family the spacing should
      // be the same as the input, and we don't select on available
      // sizes, so copy that, too.
      pResult->spacing = p->spacing;
      pResult->pixelsize = p->pixelsize;

      pResult->pFontDesc = pFont;
    }
    if (result)
      *result = FcResultMatch;
    return pResult;
  } else
  {
#ifdef MATCH_DEBUG
    printf("no best match!!!\n");
#endif
    if (result)
      *result = FcResultNoMatch;
    return NULL;
  }
}

fcExport FcBool FcPatternDel(FcPattern *p, const char *object)
{
  // This is a stub.
  return FcTrue;
}

fcExport FcObjectSet *FcObjectSetBuild(const char *first, ...)
{
  // This is a stub.
  // We assume that the object sets are always created for
  // FC_FAMILY parameters, as Mozilla uses only that.

  FcObjectSet *result = (FcObjectSet *) malloc(sizeof(FcObjectSet));

  if (result)
    memset(result, 0, sizeof(FcObjectSet));

  return result;
}

fcExport void FcObjectSetDestroy(FcObjectSet *os)
{
  // This is a stub.

  if (os)
    free(os);
}

fcExport FcBool FcInitReinitialize(void)
{
  // This is a stub

  return FcTrue;
}

fcExport FcBool FcConfigUptoDate(FcConfig *config)
{
  // This is a stub

  return FcTrue;
}

fcExport FcFontSet *FcFontSetCreate(void)
{
  FcFontSet *result = (FcFontSet *) malloc(sizeof(FcFontSet));

  if (result)
    memset(result, 0, sizeof(FcFontSet));

  return result;
}

fcExport void FcFontSetDestroy(FcFontSet *fs)
{
  if (fs)
  {
    int i;
    for (i=0; i<fs->nfont; ++i)
    {
      FcPatternDestroy(fs->fonts[i]);
    }

    if (fs->fonts)
      free(fs->fonts);

    free(fs);
  }
}

fcExport FcBool FcFontSetAdd(FcFontSet *fs, FcPattern *pat)
{
  if (fs->nfont>=fs->sfont)
  {
    void *newptr = NULL;
    fs->sfont+=32;
    newptr = realloc(fs->fonts, fs->sfont * sizeof(void *));
    if (!newptr)
    {
      fs->sfont-=32;
      return FcFalse;
    }

    fs->fonts = newptr;
  }
  fs->fonts[fs->nfont] = pat;
  fs->nfont++;
  return FcTrue;
}

fcExport FcFontSet *FcFontList(FcConfig *config, FcPattern *p, FcObjectSet *os)
{
  // This is a stub.

  // We assume that the we either have to list all fonts (pat.family==NULL),
  // or we have to list a given family (pat.family!=NULL),
  // and only their FC_FAMILY property will be queried before
  // the font list is destroyed.

  FcFontSet *result = FcFontSetCreate();

  if (!p)
    return result;

  // same logic as in FcFontMatch
  FcBool wantsMono = p->spacing == FC_MONO || ((p->family) && (stricmp("MONOSPACE", p->family)==0));
  FcBool wantsSans = ((p->family) &&
                      ((stricmp( p->family, "SWISS" ) == 0 ) ||
                       (stricmp( p->family, "HELV" ) == 0 ) ||
                       (stricmp( p->family, "SANS-SERIF" ) == 0 ) ||
                       (stricmp( p->family, "SANS" ) == 0 )
                      ));
  FcBool wantsSerif = ((p->family) &&
                       ((stricmp( p->family, "SERIF" ) == 0 ) ||
                        (stricmp( p->family, "TMS RMN" ) == 0 ) ||
                        (stricmp( p->family, DEFAULT_SERIF_FONT ) == 0 )
                       ));

  if (result)
  {
    FontDescriptionCache_p pFont;

    pFont = pFontDescriptionCacheHead;
    while (pFont)
    {
      if ((p->family==NULL) ||
          (stristr(pFont->achFamilyName, p->family)))
      {
        FcPattern *newPattern = FcPatternCreate();
        newPattern->pFontDesc = pFont;

        FcFontSetAdd(result, newPattern);
      }
      else if ((wantsMono && stricmp(pFont->achFamilyName, DEFAULT_MONOSPACED_FONT)==0) ||
               (wantsSans && stricmp(pFont->achFamilyName, DEFAULT_SANSSERIF_FONT)==0) ||
               (wantsSerif && (stricmp(pFont->achFamilyName, DEFAULT_SERIF_FONT)==0 ||
                               stricmp(pFont->achFamilyName, DEFAULT_SERIF_FONT" ")==0))
              )
      {
        FcPattern *newPattern = FcPatternCreate();
        newPattern->pFontDesc = pFont;
        FcFontSetAdd(result, newPattern);
        // here, we were obviously only searching for one
        // specific (default) font, so we can return early
        return result;
      }

      pFont = pFont->pNext;
    }
  }

  return result;
}

fcExport FcFontSet *FcFontSort(FcConfig *config, FcPattern *p, FcBool trim, FcCharSet **csp, FcResult *result)
{
  // This is a stub.

  // I currently have no idea what kind of sorting to do here,
  // what would be the best for Mozilla, so I simply won't sort it. :)
  // Use the same code as for FcFontList().

  *result = FcResultMatch;
  return (FcFontList(config, p, NULL));
}

/* Constructs a pattern representing the 'id'th font in 'file'. *
 * The number of fonts in 'file' is returned in 'count'.        */
// XXX what to do about the FcBlanks* parameter?
fcExport FcPattern *FcFreeTypeQuery(const FcChar8 *file, int id, FcBlanks *blanks, int *count)
{
  FcPattern *pattern = FcPatternCreate();
  FT_Face ftface;

  FcDefaultSubstitute(pattern);

  if (FT_New_Face(hFtLib, file, id, &ftface))
  {
    /* Could not load font. */
    FcPatternDestroy(pattern);
    *count = 0;
    return NULL;
  }
  *count = ftface->num_faces;
  pattern->family = (char*)malloc(strlen(ftface->family_name));
  strcpy(pattern->family, ftface->family_name);
  pattern->style = (char*)malloc(strlen(ftface->style_name));
  strcpy(pattern->style, ftface->style_name);

  return pattern;
}

fcExport FcBool FcPatternEqual(const FcPattern *pa, const FcPattern *pb)
{
  /* point to the same pattern */
  if (pa == pb) {
    return FcTrue;
  }

  /* one of the patterns is invalid */
  if (!pa || !pb) {
    return FcFalse;
  }

  /* check string properties */
  /* If the string have the same address or they are both NULL it would mean
   * we had equal strings. If that is not the case we have to test if only
   * one of them is NULL and finally how they compare.
   */
  if (!(pa->family == pb->family || (!pa->family && !pb->family))
      && (!pa->family || !pb->family || stricmp(pa->family, pb->family) != 0))
  {
    return FcFalse;
  }

  if (!(pa->style == pb->style || (!pa->style && !pb->style))
      && (!pa->style || !pb->style || stricmp(pa->style, pb->style) != 0))
  {
    return FcFalse;
  }

  /* check int properties */
  if (pa->weight != pb->weight ||
      pa->slant != pb->slant ||
      pa->spacing != pb->spacing ||
      pa->hintstyle != pb->hintstyle ||
      pa->rgba != pb->rgba)
  {
    return FcFalse;
  }

  /* check double properties, better not compare directly */
  if (fabs(pa->pixelsize-pb->pixelsize) > DBL_EPSILON ||
      fabs(pa->size-pb->size) > DBL_EPSILON)
  {
    return FcFalse;
  }

  /* check bool properties, just direct comparison as for ints */
  if (pa->hinting != pb->hinting ||
      pa->antialias != pb->antialias ||
      pa->embolden != pb->embolden ||
      pa->verticallayout != pb->verticallayout ||
      pa->autohint != pb->autohint)
  {
    return FcFalse;
  }

  /* if we haven't returned by now then everything is equal */
  return FcTrue;
}

