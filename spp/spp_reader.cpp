#include "spplib.h"
#include "system.h"

/* Lex a token into RESULT (external interface).  Takes care of issues
   like directive handling, token lookahead, multiple include
   optimization and skipping.  */
const cpp_token * cpp_reader::_cpp_lex_token ()
{
  cpp_token *result;

  for (;;)
    {
      if (this->cur_token == this->cur_run->limit)
	{
	  this->cur_run = this->cur_run->next_tokenrun();
	  this->cur_token = this->cur_run->base;
	}
      /* We assume that the current token is somewhere in the current
	 run.  */
      if (this->cur_token < this->cur_run->base
	  || this->cur_token >= this->cur_run->limit)
	abort ();

      if (this->lookaheads)
	{
	  this->lookaheads--;
	  result = this->cur_token++;
	}
      else
	result = _cpp_lex_direct (this);

      if (result->flags & BOL)
	{
	  /* Is this a directive.  If _cpp_handle_directive returns
	     false, it is an assembler #.  */
	  if (result->type == CPP_HASH
	      /* 6.10.3 p 11: Directives in a list of macro arguments
		 gives undefined behavior.  This implementation
		 handles the directive as normal.  */
	      && this->state.parsing_args != 1)
	    {
	      if (_cpp_handle_directive (this, result->flags & PREV_WHITE))
		{
		  if (this->directive_result.type == CPP_PADDING)
		    continue;
		  result = &this->directive_result;
		}
	    }
	  else if (this->state.in_deferred_pragma)
	    result = &this->directive_result;
	  else if (result->type == CPP_NAME
		   && (result->val.node.node->flags & NODE_MODULE)
		   && !this->state.skipping
		   /* Unlike regular directives, we do not deal with
		      tokenizing module directives as macro arguments.
		      That's not permitted.  */
		   && !this->state.parsing_args)
	    {
	      /* P1857.  Before macro expansion, At start of logical
		 line ... */
	      /* We don't have to consider lookaheads at this point.  */
	      gcc_checking_assert (!this->lookaheads);

	      cpp_maybe_module_directive (this, result);
	    }

	  if (this->cb.line_change && !this->state.skipping)
	    this->cb.line_change (this, result, this->state.parsing_args);
	}

      /* We don't skip tokens in directives.  */
      if (this->state.in_directive || this->state.in_deferred_pragma)
	break;

      /* Outside a directive, invalidate controlling macros.  At file
	 EOF, _cpp_lex_direct takes care of popping the buffer, so we never
	 get here and MI optimization works.  */
      this->mi_valid = false;

      if (!this->state.skipping || result->type == CPP_EOF)
	break;
    }

  return result;
}



const cpp_token *cpp_reader::get_token() {
    const cpp_token *result;
  /* This token is a virtual token that either encodes a location
     related to macro expansion or a spelling location.  */
  location_t virt_loc = 0;
  /* this->about_to_expand_macro_p can be overriden by indirect calls
     to functions that push macro contexts.  So let's save it so that
     we can restore it when we are about to leave this routine.  */
  bool saved_about_to_expand_macro = this->about_to_expand_macro_p;

  for (;;)
    {
      cpp_hashnode *node;
      cpp_context *context = this->context;

      /* Context->prev == 0 <=> base context.  */
      if (!context->prev)
	{
	  result = _cpp_lex_token (this);
	  virt_loc = result->src_loc;
	}
      else if (!reached_end_of_context (context))
	{
	  consume_next_token_from_context (this, &result,
					   &virt_loc);
	  if (result->flags & PASTE_LEFT)
	    {
	      paste_all_tokens (this, result);
	      if (this->state.in_directive)
		continue;
	      result = padding_token (this, result);
	      goto out;
	    }
	}
      else
	{
	  if (this->context->c.macro)
	    ++num_expanded_macros_counter;
	  _cpp_pop_context (this);
	  if (this->state.in_directive)
	    continue;
	  result = &this->avoid_paste;
	  goto out;
	}

      if (this->state.in_directive && result->type == CPP_COMMENT)
	continue;

      if (result->type != CPP_NAME)
	break;

      node = result->val.node.node;

      if (node->type == NT_VOID || (result->flags & NO_EXPAND))
	break;

      if (!(node->flags & NODE_USED)
	  && node->type == NT_USER_MACRO
	  && !node->value.macro
	  && !cpp_get_deferred_macro (this, node, result->src_loc))
	break;

      if (!(node->flags & NODE_DISABLED))
	{
	  int ret = 0;
	  /* If not in a macro context, and we're going to start an
	     expansion, record the location and the top level macro
	     about to be expanded.  */
	  if (!in_macro_expansion_p (this))
	    {
	      this->invocation_location = result->src_loc;
	      this->top_most_macro_node = node;
	    }
	  if (this->state.prevent_expansion)
	    break;

	  if ((result->flags & NO_DOT_COLON) != 0)
	    this->diagnose_dot_colon_from_macro_p = true;

	  /* Conditional macros require that a predicate be evaluated
	     first.  */
	  if ((node->flags & NODE_CONDITIONAL) != 0)
	    {
	      if (this->cb.macro_to_expand)
		{
		  bool whitespace_after;
		  const cpp_token *peek_tok = cpp_peek_token (this, 0);

		  whitespace_after = (peek_tok->type == CPP_PADDING
				      || (peek_tok->flags & PREV_WHITE));
		  node = this->cb.macro_to_expand (pfile, result);
		  if (node)
		    ret = enter_macro_context (this, node, result, virt_loc);
		  else if (whitespace_after)
		    {
		      /* If macro_to_expand hook returned NULL and it
			 ate some tokens, see if we don't need to add
			 a padding token in between this and the
			 next token.  */
		      peek_tok = cpp_peek_token (this, 0);
		      if (peek_tok->type != CPP_PADDING
			  && (peek_tok->flags & PREV_WHITE) == 0)
			_cpp_push_token_context (this, NULL,
						 padding_token (this,
								peek_tok), 1);
		    }
		}
	    }
	  else
	    ret = enter_macro_context (this, node, result, virt_loc);
	  if (ret)
 	    {
	      if (this->state.in_directive || ret == 2)
		continue;
	      result = padding_token (this, result);
	      goto out;
	    }
	}
      else
	{
	  /* Flag this token as always unexpandable.  FIXME: move this
	     to collect_args()?.  */
	  cpp_token *t = _cpp_temp_token (this);
	  t->type = result->type;
	  t->flags = result->flags | NO_EXPAND;
	  t->val = result->val;
	  result = t;
	}

      break;
    }

 out:
  if (location != NULL)
    {
      if (virt_loc == 0)
	virt_loc = result->src_loc;
      *location = virt_loc;

      if (!CPP_OPTION (this, track_macro_expansion)
	  && macro_of_context (this->context) != NULL)
	/* We are in a macro expansion context, are not tracking
	   virtual location, but were asked to report the location
	   of the expansion point of the macro being expanded.  */
	*location = this->invocation_location;

      *location = maybe_adjust_loc_for_trad_cpp (this, *location);
    }

  this->about_to_expand_macro_p = saved_about_to_expand_macro;

  if (this->state.directive_file_token
      && !this->state.parsing_args
      && !(result->type == CPP_PADDING || result->type == CPP_COMMENT)
      && !(15 & --this->state.directive_file_token))
    {
      /* Do header-name frobbery.  Concatenate < ... > as approprate.
	 Do header search if needed, and finally drop the outer <> or
	 "".  */
      this->state.angled_headers = false;

      /* Do angle-header reconstitution.  Then do include searching.
	 We'll always end up with a ""-quoted header-name in that
	 case.  If searching finds nothing, we emit a diagnostic and
	 an empty string.  */
      size_t len = 0;
      char *fname = NULL;

      cpp_token *tmp = _cpp_temp_token (this);
      *tmp = *result;

      tmp->type = CPP_HEADER_NAME;
      bool need_search = !this->state.directive_file_token;
      this->state.directive_file_token = 0;

      bool angle = result->type != CPP_STRING;
      if (result->type == CPP_HEADER_NAME
	  || (result->type == CPP_STRING && result->val.str.text[0] != 'R'))
	{
	  len = result->val.str.len - 2;
	  fname = XNEWVEC (char, len + 1);
	  memcpy (fname, result->val.str.text + 1, len);
	  fname[len] = 0;
	}
      else if (result->type == CPP_LESS)
	fname = _cpp_bracket_include (this);

      if (fname)
	{
	  /* We have a header-name.  Look it up.  This will emit an
	     unfound diagnostic.  Canonicalize the found name.  */
	  const char *found = fname;

	  if (need_search)
	    {
	      found = _cpp_find_header_unit (this, fname, angle, tmp->src_loc);
	      if (!found)
		found = "";
	      len = strlen (found);
	    }
	  /* Force a leading './' if it's not absolute.  */
	  bool dotme = (found[0] == '.' ? !IS_DIR_SEPARATOR (found[1])
			: found[0] && !IS_ABSOLUTE_PATH (found));

	  if (BUFF_ROOM (this->u_buff) < len + 1 + dotme * 2)
	    _cpp_extend_buff (this, &pfile->u_buff, len + 1 + dotme * 2);
	  unsigned char *buf = BUFF_FRONT (this->u_buff);
	  size_t pos = 0;

	  if (dotme)
	    {
	      buf[pos++] = '.';
	      /* Apparently '/' is unconditional.  */
	      buf[pos++] = '/';
	    }
	  memcpy (&buf[pos], found, len);
	  pos += len;
	  buf[pos] = 0;

	  tmp->val.str.len = pos;
	  tmp->val.str.text = buf;

	  tmp->type = CPP_HEADER_NAME;
	  XDELETEVEC (fname);

	  result = tmp;
	}
    }

  if (this->diagnose_dot_colon_from_macro_p
      && !this->about_to_expand_macro_p
      && result->type != CPP_PADDING
      && result->type != CPP_COMMENT)
    {
      if (result->type == CPP_DOT || result->type == CPP_COLON)
	cpp_error_with_line (this, CPP_DL_ERROR,
			     result->src_loc, 0,
			     "%qc in module name or partition "
			     "comes from or after macro expansion",
			     result->type == CPP_DOT ? '.' : ':');
      this->diagnose_dot_colon_from_macro_p = false;
    }

  return result;
}

