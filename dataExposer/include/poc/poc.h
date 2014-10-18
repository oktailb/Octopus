#ifndef __POC_H__
#define __POC_H__

#define PORT 8080
#define MAXNAMESIZE 256
#define MAXANSWERSIZE 4096*1024
#define POSTBUFFERSIZE 4096*1024
#define GET  0
#define POST 1

struct connection_info_struct
{
  int connectiontype;
  char *answerstring;
  struct MHD_PostProcessor *postprocessor;
};

enum basicTypes
{
    INT,
    FLOAT,
    STRING
};

enum controlerType
{
    ENTRY,
    SLIDER,
    ROTATE
};

typedef struct s_basicList
{
  void *		genericData;
  struct s_basicList*	next;
}			t_basicList;

typedef struct s_binaryList
{
  void* classifier;
  void *		genericData;
  struct s_binaryList*	left;
  struct s_binaryList*	right;
}			t_binaryList;

typedef struct s_parametrable
{
  void *	variable;
  void		(*handlerIsChanged)(void*, void*);
  char *	(*handlerValueToStr)(void*);
  void		(*handlerStrToValue)(const char *, void *);
  uint32_t	type;
  uint32_t	controler;
  uint32_t	mode;
  char *	id;
}		t_parametrable;

typedef struct	s_paramGroup
{
  char *	id;
  t_basicList *	groups;
  t_basicList * variables;
}		t_paramGroup;

typedef struct	s_ressource
{
  char *	name;
  int		fd;
  char *	data;
  char *	mimeType;
  uint32_t	size;
  void		(*generator)(t_paramGroup*, struct s_ressource*, const char*);
}		t_ressource;



typedef struct		s_uri
{
  char *		referer;
  unsigned char *	(*renderer)();
}			t_uri;

t_basicList *addToList(void *data, t_basicList *base);

static int
send_page (struct MHD_Connection *	connection,
	   const char*			page,
	   const char*			mimetype,
	   int				status_code);


static int
iterate_post (void *			coninfo_cls,
	      enum MHD_ValueKind	kind,
	      const char *		key,
              const char *		filename,
	      const char *		content_type,
              const char *		transfer_encoding,
	      const char *		data,
	      uint64_t			off,
	      size_t			size);

void request_completed (void *				cls,
			struct MHD_Connection *		connection,
			void **				con_cls,
                        enum MHD_RequestTerminationCode	toe);

int print_out_key (void *		cls,
		   enum MHD_ValueKind	kind, 
                   const char *		key,
		   const char *		value);

static int
answer_to_connection (void *			cls,
		      struct MHD_Connection *	connection,
		      const char *		url,
                      const char *		method,
		      const char *		version,
		      const char *		upload_data,
                      size_t *			upload_data_size,
		      void **			con_cls);

void		dontCare(void *oldVal, void* newVal);

void		setAsParameter(t_paramGroup *	base,
			       uint32_t *	target,
			       const char *	name,
			       void		(*handlerIsChanged)(void*, void*),
			       char *		(*handlerValueToStr)(void*),
			       void 		(*handlerStrToValue)(const char *, void *),
			       uint32_t		controler,
			       uint32_t		mode);
int hasParametrable(t_paramGroup * base, const char * id);
t_parametrable* getParametrable(const char * id);

void		addRessource(const char *name,
			     const char * fname,
			     const char * mimeType,
			     void(*generator)(t_paramGroup*, t_ressource*, const char *));
int		hasRessource(const char* name);
t_ressource *	getRessource(const char *name);



#endif
