#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>    /* dirname() */
#include <expat.h>

#include "matchbox-keyboard.h"

/* 
    <keyboard>

    <options>
       <font prefered-size=''>
       <size fixed='100x100'>
       <padding>
    </options>

    <layout id="name">
      <row>
        <key id="optional-id" obey-caps='true|false' geometry padding
	  <defualt
	     display="a"                
	     display="image:" 
	     action="utf8char"     // optional, action defulats to this    
	     action="modifier:Shift|Alt|ctrl|mod1|mod2|mod3|caps"
	     action="xkeysym:XK_BLAH"
	     action="control:">    // return etc 
	  <shifted 
	     ...... >
	  <mod1
	     ...... >
	     
	/>
        <key ... />
	<key ... />
	<spacer size="100px/%"
      </row>
    </layout>


    </keyboard>
*/

struct _keysymlookup
{
  KeySym keysym;   char *name;  
} 
MBKeyboardKeysymLookup[] =
{
 { XK_BackSpace,   "backspace" },
 { XK_Tab,	   "tab"       },
 { XK_Linefeed,    "linefeed"  },
 { XK_Clear,       "clear"     },	
 { XK_Return,      "return"    },
 { XK_Pause,       "pause" },	
 { XK_Scroll_Lock, "scrolllock" },	
 { XK_Sys_Req,     "sysreq" },
 { XK_Escape,      "escape" },	
 { XK_Delete,      "delete" },	
 { XK_Home,        "home" },
 { XK_Left,        "left" },
 { XK_Up,          "up"   },
 { XK_Right,       "right" },
 { XK_Down,        "down"  },
 { XK_Prior,       "prior" },		
 { XK_Page_Up,     "pageup" },	
 { XK_Next,        "next"   },
 { XK_Page_Down,   "pagedown" },
 { XK_End,         "end" },
 { XK_Begin,	   "begin" },
 { XK_F1,          "f1" },
 { XK_F2,          "f2" },
 { XK_F3,          "f3" },
 { XK_F4,          "f4" },
 { XK_F5,          "f5" },
 { XK_F6,          "f6" },
 { XK_F7,          "f7" },
 { XK_F8,          "f8" },
 { XK_F9,          "f9" },
 { XK_F10,         "f10" },
 { XK_F11,         "f11" },
 { XK_F12,         "f12" }
};


typedef struct MBKeyboardConfigState
{
  MBKeyboard       *keyboard;
  MBKeyboardLayout *current_layout;
  MBKeyboardRow    *current_row;
  MBKeyboardKey    *current_key;
  Bool              error;
  char             *error_msg;
}
MBKeyboardConfigState;

KeySym
mb_kbd_config_str_to_keysym(const char* str)
{
  int i;

  for (i=0; i<sizeof(MBKeyboardKeysymLookup)/sizeof(struct _keysymlookup); i++)
    if (streq(str, MBKeyboardKeysymLookup[i].name))
      return MBKeyboardKeysymLookup[i].keysym;

  return 0;
}

static unsigned char* 
config_load_file(const char* filename) 
{
  struct stat    st;
  FILE*          fp;
  unsigned char* str;
  int            len;
  char           tmp[1024];

  strncpy(tmp, filename, 1024);

  stat(tmp, &st);

#if 0  
  if (stat(tmp, &st)) 
    {
      snprintf(tmp, 1024, "%s.xml", filename);
      if (stat(tmp, &st)) 
	{/*
	  snprintf(tmp, 1024, PKGDATADIR "/%s", filename);
	  if (stat(tmp, &st)) 
	    {
	      snprintf(tmp, 1024, PKGDATADIR "/%s.xml", filename);
	      if (stat(tmp, &st)) 
		return NULL;
	    }
	 */
	}
  }  
#endif

  if (!(fp = fopen(tmp, "rb"))) return NULL;
  
  /* Read in the file. */
  str = malloc(sizeof(char)*(st.st_size + 1));
  len = fread(str, 1, st.st_size, fp);
  if (len >= 0) str[len] = '\0';
  
  fclose(fp);
  
  /* change to the same directory as the conf file - for images */
  // chdir(dirname(tmp));
  return str;
}

static const char *
attr_get_val (char *key, const char **attr)
{
  int i = 0;
  
  while (attr[i] != NULL)
    {
      if (!strcmp(attr[i], key))
	return attr[i+1];
      i += 2;
    }
  
  return NULL;
}


static void
config_handle_key_subtag(MBKeyboardConfigState *state,
			 const char            *tag,
			 const char           **attr)
{
  MBKeyboardKeyStateType keystate;
  const char            *val;
  KeySym                 found_keysym;

  /* TODO: Fix below with a lookup table 
   */
  if (streq(tag, "normal") || streq(tag, "default"))
    {
      keystate = MBKeyboardKeyStateNormal;
    }
  else if (streq(tag, "shifted"))
    {
      keystate = MBKeyboardKeyStateShifted;
    }
  else if (streq(tag, "mod1"))
    {
      keystate = MBKeyboardKeyStateMod1;
    }
  else if (streq(tag, "mod2"))
    {
      keystate = MBKeyboardKeyStateMod2;
    }
  else if (streq(tag, "mod3"))
    {
      keystate = MBKeyboardKeyStateMod3;
    }
  else
    {
      state->error = True;
      return;
    }

  if ((val = attr_get_val("display", attr)) == NULL)
    {
      state->error = True;
      return;
    }

  mb_kbd_key_set_glyph_face(state->current_key, keystate, 
			    attr_get_val("display", attr));

  if ((val = attr_get_val("action", attr)) != NULL)
    {
      /*
	     action="utf8char"     // optional, action defulats to this    
	     action="modifier:Shift|Alt|ctrl|mod1|mod2|mod3|caps"
	     action="xkeysym:XK_BLAH"
	     action="control:">    // return etc - not needed use lookup 
      */

      if (!strncmp(val, "modifier:", 9))
	{
	  /* XXX TODO XXX */
	  DBG("modifiers not handled yet\n");
	  
	}
      else if (!strncmp(val, "xkeysym:", 9))
	{
	  DBG("Checking %s\n", &val[8]);

	  found_keysym = XStringToKeysym(&val[8]);

	  if (found_keysym)
	    {
	      mb_kbd_key_set_keysym_action(state->current_key, 
					   keystate,
					   found_keysym);
	    }
	  else 
	    {
	      /* Should this error really be terminal */
	      state->error = True;
	      return;
	    }
	}
      else
	{
	  /* Its just 'regular' key  */

	  if (strlen(val) > 1  	/* match backspace, return etc */
	      && ((found_keysym  = mb_kbd_config_str_to_keysym(val)) != NULL))
	    {
	      mb_kbd_key_set_keysym_action(state->current_key, 
					   keystate,
					   found_keysym);
	    }
	  else
	    {
	      /* XXX We should actually check its a single UTF8 Char here */
	      mb_kbd_key_set_char_action(state->current_key, 
					 keystate, val);
	    }
	}
    }
  else /* fallback to reusing whats displayed  */
    {

      /* display could be an image in which case we should throw an error 
       * or summin.
      */

      mb_kbd_key_set_char_action(state->current_key, 
				 keystate, 
				 attr_get_val("display", attr));
    }

}

static void
config_handle_layout_tag(MBKeyboardConfigState *state, const char **attr)
{
  const char            *val;

  if ((val = attr_get_val("id", attr)) == NULL)
    {
      state->error = True;
      return;
    }

  state->current_layout = mb_kbd_layout_new(state->keyboard, val);

  mb_kbd_add_layout(state->keyboard, state->current_layout);
}

static void
config_handle_row_tag(MBKeyboardConfigState *state, const char **attr)
{
  state->current_row = mb_kbd_row_new(state->keyboard);
  mb_kbd_layout_append_row(state->current_layout, state->current_row);
}

static void
config_handle_key_tag(MBKeyboardConfigState *state, const char **attr)
{
  DBG("got key");

  state->current_key = mb_kbd_key_new(state->keyboard);

  mb_kbd_row_append_key(state->current_row, state->current_key);
}

static void 
config_xml_start_cb(void *data, const char *tag, const char **attr)
{
  MBKeyboardConfigState *state = (MBKeyboardConfigState *)data;

  if (streq(tag, "layout"))
    {
      config_handle_layout_tag(state, attr);
    }
  else if (streq(tag, "row"))
    {
      config_handle_row_tag(state, attr);
    }
  else if (streq(tag, "key"))
    {
      config_handle_key_tag(state, attr);
    }
  else if (streq(tag, "normal") 
	   || streq(tag, "default")
	   || streq(tag, "shifted")
	   || streq(tag, "mod1")
	   || streq(tag, "mod2")
	   || streq(tag, "mod3"))
    {
      config_handle_key_subtag(state, tag, attr);
    }

  if (state->error)
    {
      util_fatal_error("Error parsing\n");
    }
}


int
mb_kbd_config_load(MBKeyboard *kbd, char *conf_file)
{
  unsigned char         *data;
  XML_Parser             p;
  MBKeyboardConfigState *state;

  if ((data = config_load_file(conf_file)) == NULL)
    {
      fprintf(stderr, "Couldn't find '%s' device config file\n", conf_file);
      exit(1);
    }

  p = XML_ParserCreate(NULL);

  if (! p) {
    fprintf(stderr, "Couldn't allocate memory for XML parser\n");
    exit(1);
  }

  state = util_malloc0(sizeof(MBKeyboardConfigState));

  state->keyboard = kbd;

  XML_SetElementHandler(p, config_xml_start_cb, NULL);

  /* XML_SetCharacterDataHandler(p, chars); */

  XML_SetUserData(p, (void *)state);

  if (! XML_Parse(p, data, strlen(data), 1)) {
    fprintf(stderr, "XML Parse error at line %d:\n%s\n",
	    XML_GetCurrentLineNumber(p),
	    XML_ErrorString(XML_GetErrorCode(p)));
    exit(1);
  }

  return 1;
}

