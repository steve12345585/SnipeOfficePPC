/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */
#ifndef _CONNECTIVITY_EVOAB_EVOLUTION_API_HXX_
#define _CONNECTIVITY_EVOAB_EVOLUTION_API_HXX_
#include <glib-object.h>

// Initializes the API below, returns false if not available
bool EApiInit();

G_BEGIN_DECLS

// This header defined all the API methods as
// function pointers instead of real functions
// this will all evaporate as it is compiled generating
// no symbol lookups or relocations, but giving code
// clarity.

// We attempt to define a minimum API that we use:

// e-contact.h
#ifdef DECLARE_FN_POINTERS
#define EAPI_EXTERN
#else
#define EAPI_EXTERN extern
#endif


typedef void EContact;
#define E_CONTACT(a) ((EContact *)(a))
#define E_TYPE_CONTACT (e_contact_get_type())
typedef int EContactField;

EAPI_EXTERN const char      *(*e_contact_field_name)   ( EContactField field_id);
EAPI_EXTERN gpointer        (*e_contact_get)   (EContact *contact, EContactField field_id);
EAPI_EXTERN gconstpointer  (*e_contact_get_const)   (EContact *contact, EContactField field_id);
// e-source.h
typedef void ESource;
#define E_SOURCE(a) ((ESource *)(a))
EAPI_EXTERN const char  *(*e_source_peek_name)    (ESource *source);
EAPI_EXTERN const gchar *(*e_source_get_property) (ESource *source,
                       const gchar *property);

EAPI_EXTERN GType         (*e_contact_get_type) (void);
EAPI_EXTERN EContactField (*e_contact_field_id) (const char *field_name);

// e-source-list.h
typedef void ESourceList;
EAPI_EXTERN GSList *(*e_source_list_peek_groups) (ESourceList *list);

// e-source-group.h
typedef void ESourceGroup;
#define E_SOURCE_GROUP(a) ((ESourceGroup *)(a))

EAPI_EXTERN GSList  *(*e_source_group_peek_sources) (ESourceGroup *group);
EAPI_EXTERN const char *(*e_source_group_peek_base_uri)  (ESourceGroup *group);
// e-book.h
typedef enum {
  E_BOOK_QUERY_IS,
  E_BOOK_QUERY_CONTAINS,
  E_BOOK_QUERY_BEGINS_WITH,
  E_BOOK_QUERY_ENDS_WITH,
} EBookQueryTest;

typedef void EBook;
typedef void EBookQuery;

EAPI_EXTERN EBook      *(*e_book_new)       (ESource      *source,
                                                GError      **error);

EAPI_EXTERN gboolean    (*e_book_open)      (EBook       *book,
                                                 gboolean     only_if_exists,
                                              GError     **error);

EAPI_EXTERN const char *(*e_book_get_uri)   (EBook        *book);
EAPI_EXTERN ESource    *(*e_book_get_source)(EBook        *book);

EAPI_EXTERN gboolean    (*e_book_get_addressbooks)        (ESourceList **addressbook_sources,
                                                           GError      **error);

EAPI_EXTERN gboolean    (*e_book_get_contacts)            (EBook        *book,
                                                           EBookQuery   *query,
                                                           GList       **contacts,
                                                           GError      **error);

EAPI_EXTERN gboolean    (*e_book_authenticate_user)       (EBook        *book,
                                                           const char   *user,
                                                           const char   *passwd,
                                                           const char   *auth_method,
                                                           GError      **error);

// e-book-query.h
EAPI_EXTERN EBookQuery* (*e_book_query_field_exists) (EContactField   field);
EAPI_EXTERN EBookQuery* (*e_book_query_field_test)   (EContactField   field,
                                                        EBookQueryTest     test,
                                                      const char        *value);
EAPI_EXTERN EBookQuery* (*e_book_query_and)          (int nqs, EBookQuery **qs, gboolean unref);
EAPI_EXTERN EBookQuery* (*e_book_query_or)           (int nqs, EBookQuery **qs, gboolean unref);
EAPI_EXTERN EBookQuery* (*e_book_query_not)          (EBookQuery *q, gboolean unref);
EAPI_EXTERN EBookQuery* (*e_book_query_ref)          (EBookQuery *q);
EAPI_EXTERN void        (*e_book_query_unref)        (EBookQuery *q);
EAPI_EXTERN char*       (*e_book_query_to_string)    (EBookQuery *q);
EAPI_EXTERN EBookQuery* (*e_book_query_from_string)   (const char *query_string);

typedef struct {
        char *address_format; /* the two letter country code that
                                 determines the format/meaning of the
                                 following fields */
        char *po;
        char *ext;
        char *street;
        char *locality;
        char *region;
        char *code;
        char *country;
} EContactAddress;
G_END_DECLS
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
