bin_PROGRAMS = krouter
SRCS = pabuf.c str.c logging.c ketama.c md5.c command.c command_func.c message.c client.c server.c countque.c setting.c event_util.c decoder.c response.c error_str.c utime.c worker.c
krouter_SOURCES = krouter.c $(SRCS)
krouter_CFLAGS = -DPABUF_NOLOCK -DLOG_FILELINE `pkg-config --cflags glib-2.0`
#krouter_CFLAGS = -DPABUF_NOLOCK -DLOG_FILELINE `pkg-config --cflags glib-2.0` `pkg-config --cflags libevent` 
krouter_LDFLAGS = `pkg-config --static --libs glib-2.0` /usr/lib/libevent.a
#krouter_LDFLAGS = `pkg-config --static --libs glib-2.0` `pkg-config --static --libs libevent`
#ketamarouter_LDADD = `pkg-config --libs glib-2.0`
#noinst_LIBRARIES = libketamarouter.a
#libketamarouter_a_SOURCES = $(SRCS)
#libketamarouter_a_CFLAGS = $(ketamarouter_CFLAGS)
#libketamarouter_a_LIBADD = $(ketamarouter_LIBADD)
#libketamarouter_a_LDFLAGS = $(ketamarouter_LDFLAGS)
#ketamarouter_SOURCES = ketamarouter.c command.c hashtable.c murmur.c pabuffer.c protocol.c str.c
#ketamarouter_CFLAGS = -Wall -g
