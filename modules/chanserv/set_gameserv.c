/*
 * Copyright (c) 2003-2004 E. Will et al.
 * Copyright (c) 2006-2009 Atheme Development Group
 * Rights to this code are documented in doc/LICENSE.
 *
 * This file contains routines to handle the CService SET GAMESERV command.
 *
 * $Id$
 */

#include "atheme.h"

DECLARE_MODULE_V1
(
	"chanserv/set_gameserv", false, _modinit, _moddeinit,
	"$Id$",
	"Atheme Development Group <http://www.atheme.org>"
);

static void cs_cmd_set_gameserv(sourceinfo_t *si, int parc, char *parv[]);

command_t cs_set_gameserv = { "GAMESERV", N_("Allows or disallows gaming services."), AC_NONE, 2, cs_cmd_set_gameserv };

list_t *cs_set_cmdtree;
list_t *cs_helptree;

void _modinit(module_t *m)
{
	MODULE_USE_SYMBOL(cs_set_cmdtree, "chanserv/set", "cs_set_cmdtree");
	MODULE_USE_SYMBOL(cs_helptree, "chanserv/main", "cs_helptree");

        command_add(&cs_set_gameserv, cs_set_cmdtree);

	help_addentry(cs_helptree, "SET GAMESERV", "help/cservice/set_gameserv", NULL);
}

void _moddeinit()
{
	command_delete(&cs_set_gameserv, cs_set_cmdtree);

	help_delentry(cs_helptree, "SET GAMESERV");
}

static void cs_cmd_set_gameserv(sourceinfo_t *si, int parc, char *parv[])
{
	mychan_t *mc;
	const char *val;
	metadata_t *md;

	if (!(mc = mychan_find(parv[0])))
	{
		command_fail(si, fault_nosuch_target, _("Channel \2%s\2 is not registered."), parv[0]);
		return;
	}

	if (!chanacs_source_has_flag(mc, si, CA_SET))
	{
		command_fail(si, fault_noprivs, _("You are not authorized to perform this command."));
		return;
	}

	if (!strcasecmp("ALL", parv[1]))
		val = "ALL";
	else if (!strcasecmp("VOICE", parv[1]) || !strcasecmp("VOICES", parv[1]))
		val = "VOICE";
	else if (!strcasecmp("OP", parv[1]) || !strcasecmp("OPS", parv[1]))
		val = "OP";
	else if (!strcasecmp("OFF", parv[1]))
		val = NULL;
	else
	{
		command_fail(si, fault_badparams, STR_INVALID_PARAMS, "GAMESERV");
		return;
	}

	md = metadata_find(mc, "gameserv");

	if (val != NULL)
	{
		if (md != NULL && !strcasecmp(md->value, val))
		{
			command_fail(si, fault_nochange, _("\2%s\2 is already set to %s for \2%s\2."), "GAMESERV", val, mc->name);
			return;
		}
		logcommand(si, CMDLOG_SET, "%s SET GAMESERV %s", mc->name, val);

		metadata_add(mc, "gameserv", val);

		command_success_nodata(si, _("\2%s\2 has been set to \2%s\2 for \2%s\2."), "GAMESERV", val, mc->name);

		return;
	}
	else
	{
		if (md == NULL)
		{
			command_fail(si, fault_nochange, _("\2%s\2 was not set for \2%s\2."), "GAMESERV", mc->name);
			return;
		}

		logcommand(si, CMDLOG_SET, "%s SET GAMESERV OFF", mc->name);

		metadata_delete(mc, "gameserv");

		command_success_nodata(si, _("\2%s\2 has been disabled for \2%s\2."), "GAMESERV", mc->name);
	}
}

/* vim:cinoptions=>s,e0,n0,f0,{0,}0,^0,=s,ps,t0,c3,+s,(2s,us,)20,*30,gs,hs
 * vim:ts=8
 * vim:sw=8
 * vim:noexpandtab
 */