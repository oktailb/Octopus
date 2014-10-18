/*
 * System dependencies
 */
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <fnmatch.h>
#include <time.h>
#include <sys/inotify.h>

/*
 * External libraries
 */
#include <microhttpd.h>

/*
 * Project includes
 */
#include "poc.h"

//t_basicList *base = NULL;
t_basicList *baseRessources = NULL;
t_paramGroup *groups = NULL;
struct MHD_Daemon *MHDdaemon;

// TODO : la page par defaut devra etre un xml des clients,
// suivie de rss des groupes puis d'un rss des variables

const char* greatingpage = "<html><body><h1>Welcome, %s!</center></h1></body></html>";

const char* errorpage = "<html><body>This doesn't seem to be right.</body></html>";

int compareStr(void * a, void *b)
{
  return (strcmp((const char *)a, (const char *)b));
}

t_binaryList **getPlaceInTree(t_binaryList *tree, void *data, int (*compare)(void *, void *))
{
  t_binaryList **res;
  if (tree == NULL)
    res = &tree;
  else
    {
      if (compare(tree->classifier, data) <= 0)
	res = getPlaceInTree(tree->left, data, compare);
      else
	res = getPlaceInTree(tree->right, data, compare);
    }
  return (res);
}

t_binaryList *addToTree(void * data, void *classifier, t_binaryList *tree, int (*compare)(void *, void *))
{
  t_binaryList * newElement = malloc(sizeof(*newElement));
  newElement->genericData = data;
  newElement->classifier = classifier;
  t_binaryList ** parent = getPlaceInTree(tree, data, compare);
  *parent = newElement;
  return (tree);
}

t_basicList *addToList(void *data, t_basicList *base)
{
  t_basicList *newElement = malloc(sizeof(*newElement));
  newElement->genericData = data;
  newElement->next = base;
  base = newElement;
  return base;
}

static int
send_page (struct MHD_Connection *	connection,
	   const char*			page,
	   const char*			mimeType,
	   int				status_code)
{
  int			ret;
  struct MHD_Response *	response;

  response = MHD_create_response_from_buffer (strlen (page),
					      (void*) page,
					      MHD_RESPMEM_MUST_COPY);
  if (!response)
    return MHD_NO;

  MHD_add_response_header (response, "Content-Type", mimeType);
  ret = MHD_queue_response (connection, status_code, response);
  MHD_destroy_response (response);

  return ret;
}

static int
iterate_post (void *			coninfo_cls,
	      enum MHD_ValueKind	kind,
	      const char *		key,
              const char *		filename,
	      const char *		content_type,
              const char *		transfer_encoding,
	      const char *		data,
	      uint64_t			off,
	      size_t			size)
{
  kind = kind;
  transfer_encoding = transfer_encoding;
  off = off;
  struct connection_info_struct *con_info = coninfo_cls;
  
  t_basicList* tmp;
  for (tmp = groups->variables ; tmp != NULL; tmp = tmp->next)
    {
      t_parametrable *param = (t_parametrable*)(tmp->genericData);
      printf("------------------------------\n");
      printf("id = %s\n", param->id);
      
      if (0 == strcmp (key, param->id))
	{
	  if ((size > 0) && (size <= MAXNAMESIZE))
	    {
	      char *answerstring;
	      answerstring = malloc (MAXANSWERSIZE);
	      if (!answerstring) return MHD_NO;
	      
	      snprintf (answerstring, MAXANSWERSIZE, "<html><body>%s == %d</body></html>", param->id,  *((uint32_t*)param->variable));
	      con_info->answerstring = answerstring;
	    }
	  else con_info->answerstring = NULL;
	  
	  return MHD_NO;
	}
    }

  return MHD_YES;
}

void request_completed (void *				cls,
			struct MHD_Connection *		connection,
			void **				con_cls,
                        enum MHD_RequestTerminationCode	toe)
{
  cls = cls;
  connection = connection;
  toe = toe;

  struct connection_info_struct *con_info = *con_cls;

  if (NULL == con_info) return;

  if (con_info->connectiontype == POST)
    {
      MHD_destroy_post_processor (con_info->postprocessor);
      if (con_info->answerstring) free (con_info->answerstring);
    }

  free (con_info);
  *con_cls = NULL;
}

int print_out_key (void *		cls,
		   enum MHD_ValueKind	kind, 
                   const char *		key,
		   const char *		value)
{
  cls = cls;
  kind = kind;

  //  printf ("%s: %s\n", key, value);
  return MHD_YES;
}

static int
answer_to_connection (void *			cls,
		      struct MHD_Connection *	connection,
		      const char *		url,
                      const char *		method,
		      const char *		version,
		      const char *		upload_data,
                      size_t *			upload_data_size,
		      void **			con_cls)
{
  cls = cls;
  printf("URL = %s\n", url);
  MHD_get_connection_values (connection, MHD_HEADER_KIND, &print_out_key, NULL);

  if(NULL == *con_cls)
    {
      struct connection_info_struct *con_info;
      
      con_info = malloc (sizeof (struct connection_info_struct));
      if (NULL == con_info) return MHD_NO;
      con_info->answerstring = NULL;
      
      if (0 == strcmp (method, "POST"))
        {
          con_info->postprocessor
	    = MHD_create_post_processor (connection, POSTBUFFERSIZE,
                                         iterate_post, (void*) con_info);
	  
          if (NULL == con_info->postprocessor)
            {
              free (con_info);
              return MHD_NO;
            }
          con_info->connectiontype = POST;
        }
      else
        {
          con_info->connectiontype = GET;
        }
      *con_cls = (void*) con_info;
      return MHD_YES;
    }
  
  if ((0 == strcmp (method, "GET")) && (1 == hasRessource(url)))
    {
      if (hasRessource(url))
	{
          t_ressource* res = getRessource(url);
          if (res->generator != NULL)
	    res->generator(groups, res, url);
          return send_page(connection, res->data, res->mimeType, 0);
	}
      else
	printf("url = %s unknow (%d) !\n", url, hasRessource(url));
    }

  if (0 == strcmp (method, "GET"))
    {
      return send_page(connection, getRessource("root")->data, "text/html",  0);
    }

  if (0 == strcmp (method, "POST"))
    {
      struct connection_info_struct *con_info = *con_cls;

      if (*upload_data_size != 0)
        {
          MHD_post_process (con_info->postprocessor, upload_data,
			    *upload_data_size);
          *upload_data_size = 0;

          return MHD_YES;
        }
      else if (NULL != con_info->answerstring)
        return send_page (connection, con_info->answerstring, "test/html", 0);
    }

  return send_page(connection, errorpage, "test/html", 0);
}

void dontCare(void *oldVal, void* newVal)
{
  printf("###################################\n");
  printf("Value changed !\n");
  printf("###################################\n");
  oldVal = oldVal;
  newVal = newVal;
}

void setAsParameter(t_paramGroup *	base,
		    uint32_t *		target,
		    const char *	name,
		    void		(*handlerIsChanged)(void*, void*),
		    char *		(*handlerValueToStr)(void* val),
		    void 		(*handlerStrToValue)(const char *, void*),
		    uint32_t		controler,
		    uint32_t		mode)
{
  t_parametrable *first;

  first = malloc(sizeof(*first));

  first->variable = target;
  first->type = INT;
  first->mode = mode;
  first->controler = controler;
  first->id = strdup(name);
  first->handlerIsChanged = handlerIsChanged;
  first->handlerValueToStr = handlerValueToStr;
  first->handlerStrToValue = handlerStrToValue;
  base->variables = addToList(first, base->variables);
}

void addRessource(const char *name,
		  const char *fname,
		  const char * mimeType,
		  void (*generator)(t_paramGroup*, t_ressource*, const char*))
{
  t_ressource *ressource;

  ressource = malloc(sizeof(*ressource));

  ressource->name = strdup(name);
  ressource->mimeType = strdup(mimeType);
  ressource->generator = generator;
  if (generator)
    {
      ressource->data = malloc(MAXANSWERSIZE);
    }
  else
    {
      unsigned int fd = open(fname, O_RDONLY);
      struct stat st;

      inotify_add_watch(fd, fname, IN_MODIFY);
      fstat(fd, &st);
      ressource->fd = fd;
      ressource->size = st.st_size;
      ressource->data = malloc(ressource->size * sizeof(*(ressource->data)));
      read(fd, ressource->data, ressource->size);
      close(fd);
    }
  baseRessources = addToList(ressource, baseRessources);
}

t_ressource *getRessource(const char *name)
{
  t_basicList *tmp;
  for (tmp = baseRessources; tmp != NULL; tmp = tmp->next)
    {
      t_ressource *ressource = tmp->genericData;
      if (0 == fnmatch(ressource->name, name, 0))
        {
	  return (ressource);
        }
    }
  return (NULL);
}

int hasRessource(const char *name)
{
  t_basicList *tmp;
  for (tmp = baseRessources; tmp != NULL; tmp = tmp->next)
    {
      t_ressource *ressource = tmp->genericData;
      if (0 == fnmatch(ressource->name, name, 0))
        {
	  return (1);
        }
    }
  return (0);
}

void ressourceListGenerator(t_paramGroup* base, t_ressource * res, const char * url)
{
  url = url;
  memset(res->data, '\0', MAXANSWERSIZE);
  strncat(res->data, "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n", MAXANSWERSIZE);
  strncat(res->data, "<rss version=\"2.0\">\n", MAXANSWERSIZE);
  strncat(res->data, "<channel>\n", MAXANSWERSIZE);
  strncat(res->data, "<title>res->data</title>\n", MAXANSWERSIZE);
  t_basicList *tmp;
  for (tmp = baseRessources; tmp != NULL; tmp = tmp->next)
    {
      strncat(res->data, "<item>\n", MAXANSWERSIZE);
      char *buff = malloc(1024);
      memset(buff, '\0', 1024);
      t_ressource *ressource = tmp->genericData;
      snprintf(buff, 1024, "<link>%s</link>", ressource->name);
      strncat(res->data, buff, MAXANSWERSIZE);
      snprintf(buff, 1024, "<title>%s</title>", ressource->name);
      strncat(res->data, buff, MAXANSWERSIZE);
      time_t ltime;
      time(&ltime);
      snprintf(buff, 1024, "<pubDate>%s</pubDate>", ctime(&ltime));
      strncat(res->data, buff, MAXANSWERSIZE);
      strncat(res->data, "</item>\n", MAXANSWERSIZE);
    }
  strncat(res->data, "</channel>\n", MAXANSWERSIZE);
  strncat(res->data, "</rss>\n", MAXANSWERSIZE);
}

char *groupToStr(t_paramGroup* base, char * data)
{
  char *buff = malloc(1024);
  snprintf(buff, 1024, "<group name=\"%s\">\n", base->id);
  strncat(data, buff, MAXANSWERSIZE);
  t_basicList *tmp;
  for (tmp = base->variables; tmp != NULL; tmp = tmp->next)
    {
      strncat(data, "<item>\n", MAXANSWERSIZE);
      t_parametrable *param = tmp->genericData;
      snprintf(buff, 1024, "\t<link>/get_%s</link>\n", param->id);
      strncat(data, buff, MAXANSWERSIZE);
      snprintf(buff, 1024, "\t<title>%s</title>\n", param->id);
      strncat(data, buff, MAXANSWERSIZE);
      snprintf(buff, 1024, "<value>%s</value>", param->handlerValueToStr(param->variable));
      strncat(data, buff, MAXANSWERSIZE);
      strncat(data, "</item>\n", MAXANSWERSIZE);
    }
  for (tmp = base->groups; tmp != NULL; tmp = tmp->next)
    {
      t_paramGroup *grp = tmp->genericData;
      printf("avant recursion\n");
      data = groupToStr(grp, data);
      strncat(data, data, MAXANSWERSIZE);
      printf("apres recursion\n");
    }
  strncat(data, "</group>\n", MAXANSWERSIZE);
  free(buff);
  return (data);
}

void variableListGenerator(t_paramGroup * base, t_ressource * res, const char * url)
{
  url = url;
  char *buff = malloc(1024);

  memset(buff, '\0', 1024);
  memset(res->data, '\0', MAXANSWERSIZE);
  strncat(res->data, "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n", MAXANSWERSIZE);
  strncat(res->data, "<parameter version=\"1.0\">\n", MAXANSWERSIZE);
  time_t ltime;
  time(&ltime);
  snprintf(buff, 1024, "\t<pubDate>%s</pubDate>\n", ctime(&ltime));
  strncat(res->data, buff, MAXANSWERSIZE);
  snprintf(buff, 1024, "<process pid=\"%d\">\n", getpid());
  strncat(res->data, buff, MAXANSWERSIZE);

  
  groupToStr(base, res->data);
  
  strncat(res->data, "</process>\n", MAXANSWERSIZE);
  strncat(res->data, "</parameter>\n", MAXANSWERSIZE);
  free(buff);
}

void voidGenerator(t_ressource * res, const char * url)
{
  url = url;
  memset(res->data, '\0', MAXANSWERSIZE);
  sprintf(res->data, "<a href='http://perdu.com'>CLICK HERE</a>");
}

int hasParametrable(t_paramGroup * base, const char * id)
{
  t_basicList* tmp;
  for (tmp = base->variables ; tmp != NULL; tmp = tmp->next)
    {
      t_parametrable *param = (t_parametrable*)(tmp->genericData);

      if (0 == strcmp (id, param->id))
        {
	  return (1);
        }
    }
  return (0);
}

t_parametrable* getParametrable(const char * id)
{
  t_basicList* tmp;
  for (tmp = groups->variables ; tmp != NULL; tmp = tmp->next)
    {
      t_parametrable *param = (t_parametrable*)(tmp->genericData);

      if (0 == strcmp (id, param->id))
        {
	  return (param);
        }
    }
  return (NULL);
}

void setValue(t_paramGroup* base, t_ressource * res, const char * url)
{
  res = res;
  char * toParse = strdup(url);
  strtok(toParse, "_");
  char * cast = strtok(NULL, "_");
  char * name = strtok(NULL, "_"); //=> deviendra un path
  char * valueStr = strtok(NULL, "_");

  if (hasParametrable(base, name))
    {
      t_parametrable * param = getParametrable(name);
      //=> base de convertiseurs a impl.
      if (0 == strcmp(cast, "int"))
        {
	  void * oldval = param->variable;
	  param->handlerStrToValue(valueStr, param->variable);
	  if (param->handlerIsChanged)
	    param->handlerIsChanged(oldval, param->variable);
        }
    }
  free(toParse);
}

void getValue(t_paramGroup* base, t_ressource * res, const char * url)
{
  char * toParse = strdup(url);
  strtok(toParse, "_");
  char * name = strtok(NULL, "_"); //=> deviendra un path

  if (hasParametrable(base, name))
    {
      t_parametrable * param = getParametrable(name);
      //=> base de convertiseurs a impl.
      free(res->data);
      res->data = param->handlerValueToStr(param->variable);
    }
  free(toParse);
}

void addGroup(t_paramGroup *parent, t_paramGroup *newGroup)
{
  if (parent)
    parent->groups = addToList(newGroup, parent->groups);
}

char *	IntToStr(void* val)
{
  int source = *((int*) val);
  char *target = malloc(sizeof(*val) * 20);
  sprintf(target, "%d", source);
  return (target);
}

void		StrToInt(const char *str, void * target)
{
  int source = atoi(str);
  printf("parsed value == %d\n", source);
  *(int*)target = source;
}

void initParmesan()
{
  inotify_init();
  addRessource("/renderer", "my_ajax_lib.js", "text/javascript", NULL);
  addRessource("/variables", NULL, "application/xml", variableListGenerator);
  addRessource("/get*", NULL, "text/html", getValue);
  addRessource("/set*", NULL, "text/html", setValue);
  addRessource("/ressources", NULL, "application/xml", ressourceListGenerator);
  addRessource("root", "root.html", "text/html", NULL);
  
  MHDdaemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, atoi(getenv("POC_PORT")), NULL, NULL,
			     &answer_to_connection, NULL,
			     MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL,
			     MHD_OPTION_END);
  if (NULL == MHDdaemon)
    return;
}

int main (int argc, char **argv)
{
  uint32_t first = 1;
  uint32_t second = 1;
  uint32_t third = 1;
  t_paramGroup *rootGroup = malloc(sizeof(*rootGroup));
  t_paramGroup *level2Group = malloc(sizeof(*level2Group));

  level2Group->id = strdup("level2");
  level2Group->groups = NULL;
  level2Group->variables = NULL;
  rootGroup->id = strdup("root");
  rootGroup->groups = NULL;
  rootGroup->variables = NULL;
  initParmesan();
  groups = rootGroup;
  
  setAsParameter(rootGroup, &first, "first", dontCare, IntToStr, StrToInt, ENTRY, 0x00);
  setAsParameter(rootGroup, &second, "second", dontCare, IntToStr, StrToInt, ENTRY, 0x00);
  setAsParameter(level2Group, &third, "third", dontCare, IntToStr, StrToInt, ENTRY, 0x00);
  //addGroup(rootGroup, level2Group);
  while (first)
    {
      sleep(1);
      first ++;
      second --;
      third *= 2;
    }
  getchar (); 
    
  MHD_stop_daemon (MHDdaemon);
  return 0;
}

