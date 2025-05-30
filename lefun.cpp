/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: lefun.c,v 1.4 1995/01/14 00:24:55 duchier Exp $	 */
#define REV401PLUS
#ifndef lint
static char vcid[] = "$Id: lefun.c,v 1.4 1995/01/14 00:24:55 duchier Exp $";
#endif /* lint */
#define EXTERN extern
#ifdef REV401PLUS
#include "defs.h"
#endif

static long attr_missing;
static long check_func_flag;

/* Create a new psi_term on the stack with value '@' (top) and no attributes. */
ptr_psi_term stack_psi_term(long stat)
// long stat;
{
  ptr_psi_term result;

  result=STACK_ALLOC(psi_term);
  result->type=top;
  result->status=stat;
  result->flags=stat?QUOTED_TRUE:FALSE; /* 14.9 */
  result->attr_list=NULL;
  result->coref=NULL;
#ifdef TS
  result->time_stamp=global_time_stamp; /* 9.6 */
#endif
  result->resid=NULL;
  result->value_3=NULL;

  return result;
}



/* Create a new psi_term on the stack with a real number value. */
ptr_psi_term real_stack_psi_term(long stat,REAL thereal)
// long stat;
// REAL thereal;
{
  ptr_psi_term result;

  result=STACK_ALLOC(psi_term);
  result->type = (thereal==floor(thereal)) ? integer : real;
  result->status=stat;
  result->flags=stat?QUOTED_TRUE:FALSE; /* 14.9 */
  result->attr_list=NULL;
  result->coref=NULL;
#ifdef TS
  result->time_stamp=global_time_stamp; /* 9.6 */
#endif
  result->resid=NULL;
  result->value_3=(GENERIC)heap_alloc(sizeof(REAL));
  (* (REAL *)(result->value_3)) = thereal;

  return result;
}



/* Create a new psi_term on the heap with value '@' (top) and no attributes. */
ptr_psi_term heap_psi_term(long stat)
// long stat;
{
  ptr_psi_term result;

  result=HEAP_ALLOC(psi_term);
  result->type=top;
  result->status=stat;
  result->flags=stat?QUOTED_TRUE:FALSE; /* 14.9 */
  result->attr_list=NULL;
  result->coref=NULL;
#ifdef TS
  result->time_stamp=global_time_stamp; /* 9.6 */
#endif
  result->resid=NULL;
  result->value_3=NULL;

  return result;
}



/* Create an empty list on the stack,  wiped out by RM: Dec 14 1992  */
/* ptr_psi_term stack_empty_list()  is now aliased to stack_nil()    */



/******** RESIDUATE_DOUBLE(t,u)
  Residuate the current expression with T in the Residuation Variable set.
  Also store the other variable, so that its sort can be used in the
  'bestsort' calculation needed to implement disequality constraints.
*/
void residuate_double(ptr_psi_term t,ptr_psi_term u) /* 21.9 */
// ptr_psi_term t,u;
{
  ptr_resid_list curr;

  curr=STACK_ALLOC(resid_list);
  curr->var=t;
  curr->othervar=u;
  curr->next=resid_vars;
  resid_vars=curr;
}




/******** RESIDUATE(t)
  Residuate the current expression with T in the Residuation Variable set.
*/
void residuate(ptr_psi_term t)
// ptr_psi_term t;
{
  ptr_resid_list curr;

  curr=STACK_ALLOC(resid_list);
  curr->var=t;
  curr->othervar=NULL; /* 21.9 */
  curr->next=resid_vars;
  resid_vars=curr;
}



/******** RESIDUATE2(u,v)
  Residuate the current function on the two variables U and V.
*/
void residuate2(ptr_psi_term u,ptr_psi_term v)
// /ptr_psi_term u,v;
{
  residuate(u);
  if (v && u!=v) residuate(v);
}



/******** RESIDUATE3(u,v,w)
  Residuate the current function on the three variables U, V, and W.
*/
void residuate3(ptr_psi_term u,ptr_psi_term v,ptr_psi_term w)
// ptr_psi_term u,v,w;
{
  residuate(u);
  if (v && u!=v) residuate(v);
  if (w && u!=w && v!=w) residuate(w);
} 



/******** CURRY()
  Decide that the current function will have to be curried.
  This has become so simple it could be a MACRO.
  The real work is done by DO_CURRY.
*/
void curry()
{
  if (can_curry)
    curried=TRUE;
}




/******** RESIDUATEGOALONVAR(g,var,othervar)
  Add the goal to the variable's residuation list.
  Also update the residuation's 'bestsort' field if it exists (needed to
  implement complete disequality semantics).  The 'othervar' parameter
  is needed for this.
*/
long residuateGoalOnVar(ptr_goal g, ptr_psi_term var, ptr_psi_term othervar)
// ptr_goal g;
// ptr_psi_term var,othervar;
{
  long result;
  long resflag,resflag2;
  GENERIC rescode,rescode2,resvalue,resvalue2;
  /* Set to FALSE if the goal is already residuated on the var: */
  long not_found = TRUE;
  /* Points to a pointer to a residuation structure.  Used so we can */
  /* add the goal to the end of the residuation list, so that it can */
  /* can be undone later if backtracking happens.  See the call to   */
  /* push_ptr_value.  */
  ptr_residuation *r;
    
  /* 5.8 PVR */
  if ((GENERIC)var>=heap_pointer) {
    Errorline("attempt to residuate on psi-term %P in the heap.\n",var);

    return FALSE;
  }

  r= &(var->resid);
    
  while (not_found && *r) {
    if ((*r)->goal == g) { /* This goal is already attached */
      /* Keep track of best sort so far */
      /* Glb_code(..) tries to keep 'sortflag' TRUE if possible. */
      result=glb_code((*r)->sortflag,(*r)->bestsort,
		      TRUE,(GENERIC)var->type,&resflag,&rescode); //REV401PLUS cast
      result=glb_value(result,resflag,rescode,(*r)->value_2,var->value_3,
		       &resvalue); /* 6.10 */
      if (!result)
        return FALSE; /* 21.9 */
      else if (othervar) {
	result=glb_code(resflag,rescode,TRUE,(GENERIC)othervar->type, //REV401PLUS cast
			&resflag2,&rescode2);
        result=glb_value(result,resflag2,rescode2,resvalue,othervar->value_3,
			 &resvalue2); /* 6.10 */
        if (!result) {
          return FALSE;
        }
        else {
	  /* The value field only has to be trailed once, since its value */
	  /* does not change, once given. */
	  if ((*r)->value_2==NULL && resvalue2!=NULL) { /* 6.10 */
	    push_ptr_value(int_ptr,&((*r)->value_2));
	  }
	  if ((*r)->bestsort!=rescode2) {
            push_ptr_value(((*r)->sortflag?def_ptr:code_ptr),
			   &((*r)->bestsort));
            (*r)->bestsort=rescode2; /* 21.9 */
	  }
	  if ((*r)->sortflag!=resflag2) {
            push_ptr_value(int_ptr,(GENERIC *)&((*r)->sortflag)); //REV401PLUS cast
            (*r)->sortflag=resflag2; /* 21.9 */
	  }
	}
      }
      else {
	if ((*r)->value_2==NULL && resvalue!=NULL) { /* 6.10 */
	  push_ptr_value(int_ptr,&((*r)->value_2));
	}
	if ((*r)->bestsort!=rescode) {
          push_ptr_value(((*r)->sortflag?def_ptr:code_ptr),
	                 &((*r)->bestsort));
          (*r)->bestsort=rescode; /* 21.9 */
	}
	if ((*r)->sortflag!=resflag) {
          push_ptr_value(int_ptr,(GENERIC *)&((*r)->sortflag)); //REV401PLUS cast
          (*r)->sortflag=resflag; /* 21.9 */
	}
      }
      not_found = FALSE;
    }
    else
      r= &((*r)->next);  /* look at the next one */
  }
  
  if (not_found) {
    /* We must attach this goal & the variable's sort onto this variable */
    
    push_ptr_value(resid_ptr,(GENERIC *)r); // REV401PLUS cast
    *r=STACK_ALLOC(residuation);
    if (othervar) {
      result=glb_code(TRUE,(GENERIC)var->type,TRUE,(GENERIC)othervar->type,&resflag,&rescode); // REV401PLUS cast
      result=glb_value(result,resflag,rescode,var->value_3,othervar->value_3,
		       &resvalue); /* 6.10 */ // REV401PLUS casts
      if (!result) {
        return FALSE;
      }
      else {
	(*r)->sortflag=resflag;
        (*r)->bestsort=rescode; /* 21.9 */
	(*r)->value_2=resvalue; /* 6.10 */  
      }
    }
    else {
      (*r)->sortflag=TRUE;
      (*r)->bestsort=(GENERIC)var->type; /* 21.9 */
      (*r)->value_2=(GENERIC)var->value_3; /* 6.10 */
    }
    (*r)->goal=g;
    (*r)->next=NULL;
  }
  
  if (!(g->pending)) {
    /* this goal is not pending, so make sure it will be put on the goal
     * stack later
     */
    push_ptr_value(int_ptr,(GENERIC *)&(g->pending)); // REV401PLUS cast
    g->pending=(ptr_definition)TRUE; // REV401PLUS cast
  }
  
  return TRUE; /* 21.9 */
}



/******** DO_RESIDUATION()
  Undo anything that matching may have done, then
  create a residuated expression. Check that the same constraint does not
  hang several times on the same variable.

  This routine takes time proportional to the square of the number of
  residuations.  This is too slow; eventually it should be sped up, 
  especially if equality constraints are often used.
*/
long do_residuation(); /* forward declaration */

/* LIFE-defined routines reset the goal stack to what it was */
/* before the function call. */
long do_residuation_user()
{
  goal_stack=resid_aim->next; /* reset goal stack */
  return do_residuation();
}

/* C-defined routines do all stack manipulation themselves */
long do_residuation()
{
  long success;
  ptr_psi_term t,u;
  ptr_goal *gs;
  
  /* This undoes perfectly valid work! */
  /* The old version of Wild_Life did not trail anything
     during matching, so I think this was a nop for it. */
  /* PVR 11.5 undo(resid_limit); */
  /* PVR 11.5 choice_stack=cut_point; */

  /* PVR 9.2.94 */
  /* goal_stack=resid_aim->next; */

  if (trace) {
    tracing();
    print_resid_message(resid_aim->aaaa_1,resid_vars);
  }

  while (resid_vars) {
    
    t=resid_vars->var; /* 21.9 */
    u=resid_vars->othervar; /* 21.9 */
    /* PVR */ deref_ptr(t);
    resid_vars=resid_vars->next;
    Traceline("residuating on %P (other = %P)\n",t,u);
    
    success=residuateGoalOnVar(resid_aim, t, u); /* 21.9 */
    if (!success) { /* 21.9 */
      Traceline("failure because of disentailment\n");
      return FALSE;
    }
  }
  
  Traceline("no failure because of disentailment\n");
  return TRUE; /* 21.9 */
}



/********* DO_CURRYING()
  This performs CURRYing: all that needs to be done is to yield the calling
  term as the result after having given up on evaluation. In effect the calling
  psi-term is left intact.
*/
void do_currying()
{
  ptr_psi_term funct,result;

  /* PVR 5.11 undo(resid_limit); */
  /* PVR 5.11 choice_stack=cut_point; */
  goal_stack=resid_aim->next;
  funct=(ptr_psi_term )resid_aim->aaaa_1;
  result=(ptr_psi_term )resid_aim->bbbb_1;
    
  Traceline("currying %P\n",funct);
   
  push_goal(unify_noeval,funct,result,NULL);
  resid_aim=NULL;
}



/******** RELEASE_RESID(t)
  Release the residuations pending on the Residuation Variable T.
  This is done by simply pushing the residuated goals onto the goal-stack.
  A goal is not added if already present on the stack.
  Two versions of this routine exist: one which trails t and one which never
  trails t.
*/
void release_resid_main(ptr_psi_term t,long trailflag)
// ptr_psi_term t;
// long trailflag;
{
  ptr_goal g;
  ptr_residuation r;
  
  if (r=t->resid) {
    if (trailflag) push_ptr_value(resid_ptr,(GENERIC *)&(t->resid)); // REV401PLUS cast
    t->resid=NULL;
    
    while (r) {
      g=r->goal;
      if (g->pending) {
	
	push_ptr_value(int_ptr,(GENERIC *)&(g->pending)); // REV401PLUS cast
	g->pending=FALSE;
	
	push_ptr_value(goal_ptr,(GENERIC *)&(g->next)); // REV401PLUS cast
	
	g->next=goal_stack;
	goal_stack=g;
	
        Traceline("releasing %P\n",g->aaaa_1);
      }
      r=r->next;
    }
  }
}

void release_resid(ptr_psi_term t)
// ptr_psi_term t;
{
  release_resid_main(t,TRUE);
}

void release_resid_notrail(ptr_psi_term t)
// ptr_psi_term t;
{
  release_resid_main(t,FALSE);
}



/******** APPEND_RESID(u,v)
  Append the residuations pending on V to U. This routine does not check that
  the same constraint is not present twice in the end on U. This doesn't matter
  since RELEASE_RESID ensures that the same constraint is not released more
  than once.
*/
void append_resid(ptr_psi_term u,ptr_psi_term v)
// ptr_psi_term u,v;
{
  ptr_residuation *g;
  
  g= &(u->resid);
  while (*g)
    g = &((*g)->next);
  
  push_ptr_value(resid_ptr,(GENERIC *)g); // REV401PLUS cast
  *g=v->resid;
}



/******** EVAL_AIM()
  Evaluate a function.
  This copies the current definition of the function and
  stacking the various goals that are necessary to evaluate the function
  correctly.
  It creates an extra psi-term (with value top) in which to write the result.
*/
long eval_aim()
{
  long success=TRUE;
  ptr_psi_term funct,result,head,body;
  ptr_pair_list rule;
  /* RESID */ ptr_resid_block rb;
  ptr_choice_point cutpt;
  ptr_psi_term match_date; /* 13.6 */
  
  funct=(ptr_psi_term )aim->aaaa_1;
  deref_ptr(funct);

  /*  RM: Jun 18 1993  */
  push2_ptr_value(int_ptr,(GENERIC *)&(funct->status),(GENERIC)(funct->status & SMASK)); // REV401PLUS cast
  funct->status=4;

  
  /* if (!funct->type->evaluate_args) mark_quote(funct); 25.8 */ /* 18.2 PVR */
  result=(ptr_psi_term )aim->bbbb_1;
  rule=(ptr_pair_list )aim->cccc_1;

  match_date=(ptr_psi_term )stack_pointer;
  cutpt=choice_stack; /* 13.6 */

  /* For currying and residuation */
  curried=FALSE;
  can_curry=TRUE;
  /* resid_aim=aim; */
  resid_vars=NULL;
  /* resid_limit=(ptr_goal )stack_pointer; 12.6 */
      
  if (rule) {
    Traceline("evaluate %P\n",funct);
    if ((unsigned long)rule<=MAX_BUILT_INS) {
      
      resid_aim=aim;
      success=c_rule[(unsigned long)rule]();

      if (curried)
	do_currying();
      else
	if (resid_vars)
	  success=do_residuation(); /* 21.9 */
	else {
	  /* resid_aim=NULL; */
        }
    }
    else {
      while (rule && (rule->aaaa_2==NULL || rule->bbbb_2==NULL)) {
        rule=rule->next;
        Traceline("alternative rule has been retracted\n");
      }
      if (rule) {
        /* push_choice_point(eval,funct,result,rule->next); */ /* 17.6 */

        resid_aim=aim;
        /* RESID */ rb = STACK_ALLOC(resid_block);
        /* RESID */ save_resid(rb,match_date);
        /* RESID */ /* resid_aim = NULL; */

        clear_copy();

	/*  RM: Jun 18 1993: no functions in head */
	/*  if (TRUE)
	    head=eval_copy(rule->a,STACK);
	    else */
	
	head=quote_copy(rule->aaaa_2,STACK);
        body=eval_copy(rule->bbbb_2,STACK);
	head->status=4;
	
        if (rule->next) /* 17.6 */
          push_choice_point(eval,funct,result,(GENERIC)rule->next); // REV401PLUS cast

        push_goal(unify,body,result,NULL);
        /* RESID */ push_goal(eval_cut,body,(ptr_psi_term)cutpt,(GENERIC)rb); /* 13.6 */ // REV401PLUS casts
        /* RESID */ push_goal(match,funct,head,(GENERIC)rb); // REV401PLUS cast
        /* eval_args(head->attr_list); */
      }
      else {
        success=FALSE;
        /* resid_aim=NULL; */
      }
    }
  }
  else {
    success=FALSE;
    /* resid_aim=NULL; */
  }
  resid_aim=NULL;
  /* match_date=NULL; */ /* 13.6 */
  return success;
}



/* Match the corresponding arguments */
/* RESID */ void match_attr1(ptr_node *u,ptr_node v,ptr_resid_block rb) //REV401PLUS add void 
	    //ptr_node *u,v;
	    /* RESID */ // ptr_resid_block rb;
{
  long cmp;
  ptr_node temp;
  
  if (v) {
    if (*u==NULL)
      attr_missing=TRUE;
    else {
      prt("lefun featcmp 1");
      cmp=featcmp((*u)->key,v->key);
      if(cmp==0) {
        ptr_psi_term t;
  	/* RESID */ match_attr1(&((*u)->right),v->right,rb);
        t = (ptr_psi_term) (*u)->data;
  	/* RESID */ push_goal(match,(ptr_psi_term)(*u)->data,(ptr_psi_term)v->data,(GENERIC)rb); // REV401PLUS casts
        /* deref2_eval(t); */
  	/* RESID */ match_attr1(&((*u)->left),v->left,rb);
      }
      else if (cmp>0) {
        temp=v->right;
        v->right=NULL;
  	/* RESID */ match_attr1(u,temp,rb);
  	/* RESID */ match_attr1(&((*u)->left),v,rb);
  	v->right=temp;
      }
      else {
  	temp=v->left;
  	v->left=NULL;
  	/* RESID */ match_attr1(&((*u)->right),v,rb);
  	/* RESID */ match_attr1(u,temp,rb);
  	v->left=temp;
      }
    }
  }
}


/* Evaluate the lone arguments (for lazy failure + eager success) */
/* RESID */ void match_attr2(ptr_node *u,ptr_node v,ptr_resid_block rb) // REV401PLUS add void
	    // ptr_node *u,v;
	    /* RESID */ //ptr_resid_block rb;
{
  long cmp;
  ptr_node temp;
  
  if (v) {
    if (*u==NULL) { /* PVR 12.03 */
      ptr_psi_term t;
      match_attr1(u,v->right,rb);
      t = (ptr_psi_term) v->data;
      deref2_rec_eval(t);
      match_attr1(u,v->left,rb);
    }
    else {
      prt("lefun featcmp 2");
      cmp=featcmp((*u)->key,v->key);
      if(cmp==0) {
  	/* RESID */ match_attr2(&((*u)->right),v->right,rb);
  	/* RESID */ match_attr2(&((*u)->left),v->left,rb);
      }
      else if (cmp>0) {
        temp=v->right;
        v->right=NULL;
  	/* RESID */ match_attr2(u,temp,rb);
  	/* RESID */ match_attr2(&((*u)->left),v,rb);
  	v->right=temp;
      }
      else {
  	temp=v->left;
  	v->left=NULL;
  	/* RESID */ match_attr2(&((*u)->right),v,rb);
  	/* RESID */ match_attr2(u,temp,rb);
  	v->left=temp;
      }
    }
  }
  else if (*u!=NULL) {
    ptr_psi_term t /* , empty */ ;
    match_attr1(&((*u)->right),v,rb);
    t = (ptr_psi_term) (*u)->data;
    /* Create a new psi-term to put the (useless) result: */
    /* This is needed so that *all* arguments of a function call */
    /* are evaluated, which avoids incorrect 'Yes' answers.      */
    deref2_rec_eval(t); /* Assumes goal_stack is already restored. */
    match_attr1(&((*u)->left),v,rb);
  }
}


/* Evaluate the corresponding arguments */
/* RESID */ void match_attr3(ptr_node *u,ptr_node v,ptr_resid_block rb) // REV401PLUS add void
	    //ptr_node *u,v;
	    /* RESID */ //ptr_resid_block rb;
{
  long cmp;
  ptr_node temp;
  
  if (v) {
    if (*u==NULL)
      attr_missing=TRUE;
    else {
      prt("lefun featcmp 3");
      cmp=featcmp((*u)->key,v->key);
      if(cmp==0) {
        ptr_psi_term t1,t2;
  	/* RESID */ match_attr3(&((*u)->right),v->right,rb);
        t1 = (ptr_psi_term) (*u)->data;
        t2 = (ptr_psi_term) v->data;
  	/* RESID */ /* push_goal(match,(*u)->data,v->data,rb); */
        deref2_eval(t1); /* Assumes goal_stack is already restored. */
        deref2_eval(t2); /* PVR 12.03 */
  	/* RESID */ match_attr3(&((*u)->left),v->left,rb);
      }
      else if (cmp>0) {
        temp=v->right;
        v->right=NULL;
  	/* RESID */ match_attr3(u,temp,rb);
  	/* RESID */ match_attr3(&((*u)->left),v,rb);
  	v->right=temp;
      }
      else {
  	temp=v->left;
  	v->left=NULL;
  	/* RESID */ match_attr3(&((*u)->right),v,rb);
  	/* RESID */ match_attr3(u,temp,rb);
  	v->left=temp;
      }
    }
  }
}



/******** MATCH_ATTR(u,v)
  Match the attribute trees of psi_terms U and V.
  If V has an attribute that U doesn't then curry.
  U is the calling term, V is the definition.
  This routine is careful to push nested eval and match goals in
  descending order of feature names.
*/
void match_attr(ptr_node *u,ptr_node v,ptr_resid_block rb)
//ptr_node *u,v;
//ptr_resid_block rb;
{
  match_attr1(u,v,rb); /* Match corresponding arguments (third) */
  match_attr2(u,v,rb); /* Evaluate lone arguments (second) */
  match_attr3(u,v,rb); /* Evaluate corresponding arguments (first) */
}





/******** MATCH_AIM()
  This is very similar to UNIFY_AIM, only matching cannot modify the calling
  psi_term.   The first argument is the calling term (which may not be changed)
  and the second argument is the function definition (which may be changed).
  Residuate the expression if the calling term is more general than the
  function definition.
*/
long match_aim()
{
  long success=TRUE;
  ptr_psi_term u,v,tmp;
  REAL r;
  long less,lesseq;
  ptr_resid_block rb;
  ptr_psi_term match_date;
  
  u=(ptr_psi_term )aim->aaaa_1;
  v=(ptr_psi_term )aim->bbbb_1;
  deref_ptr(u);
  deref_ptr(v);
  rb=(ptr_resid_block)aim->cccc_1;
  restore_resid(rb,&match_date);
  
  if (u!=v) {
    if (success=matches(u->type,v->type,&lesseq)) {
      if (lesseq) {
        if (u->type!=cut || v->type!=cut) { /* Ignore value field for cut! */
          if (v->value_3) {
            if (u->value_3) {
              if (overlap_type(v->type,real))
                success=(*((REAL *)u->value_3)==(*((REAL *)v->value_3)));
              else if (overlap_type(v->type,quoted_string))
                success=(strcmp((char *)u->value_3,(char *)v->value_3)==0);
	      /* DENYS: BYTEDATA */
              else if (overlap_type(v->type,sys_bytedata)) {
		unsigned long ulen = *((unsigned long *) u->value_3);
		unsigned long vlen = *((unsigned long *) v->value_3);
                success=(ulen==vlen && memcmp((char *)u->value_3,(char *)v->value_3,ulen)==0);  // was bcmp in Linux
	      }
            }
            else
              residuate_double(u,v);
          }
        }
      }
      else if (u->value_3) {
        /* Here we have U <| V but U and V have values which cannot match. */
        success=TRUE;
          
        if (v->value_3) {
          if (overlap_type(v->type,real))
            success=(*((REAL *)u->value_3)==(*((REAL *)v->value_3)));
        }
        else if (overlap_type(u->type,integer)) {
          r= *((REAL *)u->value_3);
          success=(r==floor(r));
        }
          
        if (success) residuate_double(u,v);
      } 
      else
        residuate_double(u,v);
                  
      if (success) {
        if (FUNC_ARG(u) && FUNC_ARG(v)) { /*  RM: Feb 10 1993  */
          /* residuate2(u,v); 21.9 */
          residuate_double(u,v); /* 21.9 */
          residuate_double(v,u); /* 21.9 */
	}
        else if (FUNC_ARG(v)) {  /*  RM: Feb 10 1993  */
          residuate_double(v,u); /* 21.9 */
        }
        else {
          v->coref=u;
        } /* 21.9 */
	  attr_missing=FALSE;
	  match_attr(&(u->attr_list),v->attr_list,rb);
	  if (attr_missing) {
            if (can_curry)
              curried=TRUE;
            else
              residuate_double(u,v);
          }
        /* } 21.9 */
      }
    }
  }

  can_curry=FALSE;
  save_resid(rb,match_date); /* updated resid_block */
  /* This should be a useless statement: */
  resid_aim = NULL;
  
  return success;
}



/******************************************************************************
  The following routines prepare terms for unification, proof or matching.
  They deal with conjunctions, disjunctions, functions and arguments which
  have to be examined before the general proof can continue.
*/



/* Forward declarations */
//long check_out();
//long eval_args();



/******** EVAL_ARGS(n)
  N is an attribute tree, the attributes must be examined, if any reveal
  themselves to need evaluating then return FALSE.
*/
long i_eval_args(ptr_node n)
// ptr_node n;
{
  check_func_flag=FALSE;
  return eval_args(n);
}



long eval_args(ptr_node n)
// ptr_node n;
{
  long flag=TRUE;
  
  if (n) {
    flag = eval_args(n->right);
    flag = check_out((ptr_psi_term)n->data) && flag; // REV401PLUS cast
    flag = eval_args(n->left) && flag;
  }
  
  return flag;
}



/******** CHECK_DISJ(t)
  Deal with disjunctions.
*/
void check_disj(ptr_psi_term t)
// ptr_psi_term t;
{
  Traceline("push disjunction goal %P\n",t);
  if (t->value_3) 
    push_goal(disj,t,t,(GENERIC)TRUE); /* 18.2 PVR */
  else
    push_goal(fail,NULL,NULL,NULL);
}



/******** CHECK_FUNC(t)
  Deal with an unevaluated function: push an 'eval' goal for it, which will
  cause it to be evaluated.
*/
void check_func(ptr_psi_term t)
//ptr_psi_term t;
{
  ptr_psi_term result,t1,copy;

  /* Check for embedded definitions
     RM: Dec 16 1992  Re-instated this check then disabled it again
     if (resid_aim) {
     Errorline("embedded functions appeared in %P.\n",resid_aim->a);
     fail_all();
     }
     else */ {
    
    Traceline("setting up function call %P\n",t);
    /* Create a psi-term to put the result */
    result = stack_psi_term(0);
    
    /* Make a partial copy of the calling term */
    copy=stack_copy_psi_term(*t);
    copy->status &= ~RMASK;
  
    /* Bind the calling term to the result */
    /* push_ptr_value(psi_term_ptr,&(t->coref)); */
    push_psi_ptr_value(t,(GENERIC *)&(t->coref)); // REV401PLUS cast
    t->coref=result;

    /* Evaluate the copy of the calling term */
    push_goal(eval,copy,result,(GENERIC)t->type->rule); // REV401PLUS cast
  
    /* Avoid evaluation for built-in functions with unevaluated arguments */
    /* (cond and such_that) */
    check_func_flag=TRUE;
    if (t->type==iff) {
      get_one_arg(t->attr_list,&t1);
      if (t1) {
	/* mark_eval(t1); 24.8 */
        check_out(t1);
      }
    }
    else if(t->type==disjunction) {
    }
    else if (t->type!=such_that) {
      if (t->type->evaluate_args)
        eval_args(t->attr_list);
      /* else mark_quote_tree(t->attr_list); 24.8 25.8 */
    }
  }
}




/******** CHECK_TYPE(t)
  Here we deal with a type which may need checking.
  This routine will have to be modified to deal with the infinite loops
  currently caused by definitions such as:

  :: H:husband(spouse => wife(spouse => H)).
  :: W:wife(spouse => husband(spouse => W)).

*/
long check_type(ptr_psi_term t)
// ptr_psi_term t;
{
  long flag=FALSE;

  push2_ptr_value(int_ptr,(GENERIC *)&(t->status),(GENERIC)(t->status & SMASK)); // REV401PLUS casts
  /* push_ptr_value(int_ptr,&(t->status)); */
  
  if (t->type->properties) {
    if (t->attr_list || t->type->always_check) {
      /* Check all constraints here: */
      fetch_def(t, TRUE); /* PVR 18.2.94 */
      /* t->status=(2 & SMASK) | (t->status & RMASK); PVR 18.2.94 */

      eval_args(t->attr_list);
      flag=FALSE;
    }
    else {
      /* definition pending on more information */
      t->status= (2 & SMASK) | (t->status & RMASK);
      flag=TRUE;
    }
  }
  else {
    
    /*  RM: Dec 15 1992  I don't know what this is for
	if (!ovverlap_type(t->type,alist))
	t->status= (4 & SMASK) | (t->status & RMASK);
	*/
    
    flag=eval_args(t->attr_list);
  }
  
  return flag;
}


  
/******** CHECK_OUT(t)
  This routine checks out psi_term T.
  It deals with the following cases:
  - T is a conjunction,
  - T is a type which has properties to check.
  - The same for T's arguments.
  If any of the above holds then proof has to be suspended until the
  case has been dealt with.  This is done by pushing goals on the goal_stack
  to handle the case.  If all is dealt with then CHECK_OUT returns TRUE.
  I.e., CHECK_OUT returns TRUE iff it has not pushed any goals on the stack.

  Evaluation is *not* done here, but as a part of dereferencing when a value
  is needed.

  Of all the routines related to check_out, only i_check_out, check_func,
  i_eval_args, and the dereference routines are called from outside of this
  file (lefun.c).
  - i_check_out(t) checks out everything except functions.  When a function
    is encountered, check_out returns immediately without looking inside it.
  - f_check_out(t) checks out functions too.
  - i_eval_args(n) checks out all arguments, except functions.
  - check_func(t) checks out a function & all its arguments (including all
    nested functions.  This is done as part of dereferencing, which is part
    of unification, matching, built-ins, and user-defined routines.
*/
long i_check_out(ptr_psi_term t)
// ptr_psi_term t;
{
  check_func_flag=FALSE;
  return check_out(t);
}

long f_check_out(ptr_psi_term t)
// ptr_psi_term t;
{
  check_func_flag=TRUE;
  return check_out(t);
}

long check_out(ptr_psi_term t)
// ptr_psi_term t;
{
  long flag=FALSE;
  static int cnt = 0;
  
 
  deref_ptr(t);

  /* Traceline("PVR: entering check_out with status %d and term %P\n",
            t->status,t); for brunobug.lf PVR 14.2.94 */

  if (t->status || (GENERIC)t >= heap_pointer) /*  RM: Feb  8 1993  */
  {
      prt("lefun check out A");
      
      flag = TRUE;
  }
  else {
    t->status |= RMASK;

    switch(t->type->wl_type) { /*  RM: Feb  8 1993  */
      
    case function_it:
        prt("lefun check out case function TOP");
      if (check_func_flag) {
          prt("lefun check out case function IF");
	check_func(t);
	flag=TRUE;
      }
      else {
          prt("lefun check out case function ELSE");
	/* Function evaluation handled during matching and unification */
	flag=TRUE;
      }
      break;

    case type_it:
        prt("lefun check out case TYPE");
      flag=check_type(t);
      break;

    case global_it: /*  RM: Feb  8 1993  */
        prt("lefun check out case GLOBAL");
        eval_global_var(t);
       check_out(t);
      flag=FALSE;
      break;
      
    default:
        prt("lefun check out case function DEFAULT");
      flag=eval_args(t->attr_list);
    }
    t->status &= ~RMASK;
  }
 
  return flag;	
}



/********************************************************************/
/*                                                                  */
/* New dereference routines for Wild_Life                           */
/* These routines handle evaluation-by-need.  Check_out is changed  */
/* to no longer call check_func, which is done in the new routines. */
/* Functions inside of psi-terms are only evaluated if needed.  It  */
/* is assumed that 'needed' is true when they are derefed.          */
/*                                                                  */
/* There are three new dereference routines:                        */
/*    deref_eval(P)                                                 */
/*       If the psi-term P is a function, call check_func to        */
/*       push eval goals so that the function will be evaluated.    */
/*       Then return TRUE so that the caller can itself return.     */
/*       This only looks at the top level.                          */
/*    deref_rec(P)                                                  */
/*       If the psi-term P recursively contains any functions, then */
/*       push eval goals to evaluate all of them.  Set a global     */
/*       variable deref_flag if this is the case.                   */
/*    deref_args(P,S)                                               */
/*       Same as above, except does not look at the top level or at */
/*       the arguments named in the set S.                          */
/*       This is needed to guarantee evaluation of all arguments of */
/*       a built-in, even those not used by the built-in.           */
/*                                                                  */
/* The original dereference macro is renamed to:                    */
/*    deref_ptr(P) = while (P->coref) P=P->coref                    */
/* There are three new macros:                                      */
/*    deref(P)        = deref_ptr(P);                               */
/*                      if (deref_eval(P)) then return TRUE         */
/*    deref_rec(P)    = deref_ptr(P);                               */
/*                      if (deref_rec_eval(P)) then return TRUE     */
/*    deref_args(P,S) = deref_ptr(P);                               */
/*                      if (deref_args_eval(P,S)) then return TRUE  */
/*                                                                  */
/********************************************************************/

static long deref_flag;
void deref_rec_body();
void deref_rec_args();
void deref_rec_args_exc();

/* Ensure evaluation of top of psi-term */
long deref_eval(ptr_psi_term t)
// ptr_psi_term t;
{
  ptr_goal save=goal_stack;

  deref_flag=FALSE;
  goal_stack=aim;

  if (t->status==0) {
      if (t->type->wl_type == function_it) {
      check_func(t);    /* Push eval goals to evaluate the function. */
      deref_flag=TRUE;  /* TRUE so that caller will return to main_prove. */
    }
    else
      if(t->type->wl_type==global_it) { /*  RM: Feb 10 1993  */
	eval_global_var(t);
	deref_ptr(t);/*  RM: Jun 25 1993  */
	deref_flag=deref_eval(t);
      }
      else {
	if (t->status!=2) {
	  if((GENERIC)t<heap_pointer)
	    push_ptr_value(int_ptr,(GENERIC *)&(t->status)); /*  RM: Jul 15 1993  */ // REV401PLUS cast
	  t->status=4;
	  deref_flag=FALSE;
	}
      }
  }
  else
    deref_flag=FALSE;
  
  if (!deref_flag) goal_stack=save;
  return (deref_flag);
}

/* Ensure evaluation of *all* of psi-term */
long deref_rec_eval(ptr_psi_term t)
// ptr_psi_term t;
{
  ptr_goal save=goal_stack;

  deref_flag=FALSE;
  goal_stack=aim;
  deref_rec_body(t);
  if (!deref_flag) goal_stack=save;
  return (deref_flag);
}

void deref_rec_body(ptr_psi_term t)
// ptr_psi_term t;
{
  if (t->status==0) {
    if (t->type->wl_type==function_it) {
      check_func(t);
      deref_flag=TRUE;
    }
    else
      if(t->type->wl_type==global_it) { /*  RM: Feb 10 1993  */
	eval_global_var(t);
	deref_ptr(t);/*  RM: Jun 25 1993  */
	deref_rec_body(t);
      }
      else {
	/* if (t->status!=2) Tried adding this -- PVR 9.2.94 */
	  if((GENERIC)t<heap_pointer)
	    push_ptr_value(int_ptr,(GENERIC *)&(t->status));/*  RM: Jul 15 1993  */ // REV401PLUS cast
	  t->status=4;
	  deref_rec_args(t->attr_list);
      }
  }
}

void deref_rec_args(ptr_node n)
// ptr_node n;
{
  ptr_psi_term t1;
  
  if (n) {
    deref_rec_args(n->right);
    t1 = (ptr_psi_term) (n->data);
    deref_ptr(t1);
    deref_rec_body(t1);
    deref_rec_args(n->left);
  }
}

/* Same as deref_rec_eval, but doesn't look at either the top level or */
/* the arguments in the set. */
long deref_args_eval(ptr_psi_term t,long set)
// ptr_psi_term t;
// long set;
{
  ptr_goal save = goal_stack;
  ptr_goal top = aim;

  deref_flag = FALSE;
  goal_stack = top;
  deref_rec_args_exc(t->attr_list,set);
  if (!deref_flag) goal_stack = save;
  return (deref_flag);
}

/* Return TRUE iff string (considered as number) is in the set */
/* This routine only recognizes the strings "1", "2", "3",     */
/* represented as numbers 1, 2, 4.                             */
long in_set(char *str,long set)
// char *str;
// long set;
{
  prt("lefun featcmp 4");
  if (set&1 && !featcmp(str,str_constants->ONE)) return TRUE;
  prt("lefun featcmp 5");
  if (set&2 && !featcmp(str,str_constants->TWO)) return TRUE;
  prt("lefun featcmp 6");
  if (set&4 && !featcmp(str,str_constants->THREE)) return TRUE;
  prt("lefun featcmp 7");
  if (set&8 && !featcmp(str,str_constants->FOUR)) return TRUE;
  return FALSE;
}

void deref_rec_args_exc(ptr_node n,long set)
// ptr_node n;
//long set;
{
  ptr_psi_term t;
  
  if (n) {
    deref_rec_args_exc(n->right,set);
    if (!in_set(n->key,set)) {
      t = (ptr_psi_term) (n->data);
      deref_ptr(t);
      deref_rec_body(t);
    }
    deref_rec_args_exc(n->left,set);
  }
}


/* These two needed only for match_aim and match_attr: */

/* Same as deref_eval, but assumes goal_stack already restored. */
void deref2_eval(ptr_psi_term t)
//ptr_psi_term t;
{
  deref_ptr(t);
  if (t->status==0) {
    if (t->type->wl_type==function_it) {
      check_func(t);
    }
    else 
      if(t->type->wl_type==global_it) { /*  RM: Feb 10 1993  */
      	eval_global_var(t);
	deref_ptr(t);/*  RM: Jun 25 1993  */
	deref2_eval(t);
      }
      else {
	t->status=4;
      }
  }
}

/* Same as deref_rec_eval, but assumes goal_stack already restored. */
void deref2_rec_eval(ptr_psi_term t)
// ptr_psi_term t;
{
  deref_ptr(t);
  deref_rec_body(t);
}

/********************************************************************/

/* Saving & restoring residuation information */

void save_resid(ptr_resid_block rb,ptr_psi_term match_date)
// ptr_resid_block rb;
// ptr_psi_term match_date;
{
   if (rb) {
      rb->cc_cr = (can_curry<<1) + curried; /* 11.9 */
      rb->ra = resid_aim;
      rb->rv = resid_vars;
      /* rb->cr = curried; 11.9 */
      /* rb->cc = can_curry; 11.9 */
      rb->md = match_date;
   }
}

void restore_resid(ptr_resid_block rb,ptr_psi_term *match_date)
// ptr_resid_block rb;
// ptr_psi_term *match_date;
{
   if (rb) {
      can_curry = (rb->cc_cr&2)?TRUE:FALSE; /* 11.9 */
      curried   = (rb->cc_cr&1)?TRUE:FALSE; /* 11.9 */
      resid_aim = rb->ra;
      resid_vars = rb->rv;
      /* curried = rb->cr; 11.9 */
      /* can_curry = rb->cc; 11.9 */
      *match_date = rb->md;
   }
}



/******** EVAL_GLOBAL_VAR(t)
  Dereference a global variable.
  */

void eval_global_var(ptr_psi_term t)     /*  RM: Feb 10 1993  */

//     ptr_psi_term t;
{
  deref_ptr(t);

  /* Global variable (not persistent) */

  Traceline("dereferencing variable %P\n",t);
  
  /* Trails the heap RM: Nov 10 1993  */
  if(!t->type->global_value) {

    /* Trail the heap !! */
    {
      ptr_stack n;
      n=STACK_ALLOC(stack);
      n->type=psi_term_ptr;
      n->aaaa_3= (GENERIC *) &(t->type->global_value);
      n->bbbb_3= NULL;
      n->next=undo_stack;
      undo_stack=n;
    }


    clear_copy();
    t->type->global_value=eval_copy(t->type->init_value,STACK);

  }

  /* var_occurred=TRUE; RM: Feb  4 1994  */

  if(t->type->wl_type==global_it && t!=t->type->global_value) {
    /*Traceline("dereferencing variable %P\n",t);*/
    push_psi_ptr_value(t,(GENERIC *)&(t->coref)); // REV401PLUS cast
    t->coref=t->type->global_value;
  }
}




/******** INIT_GLOBAL_VARS()
  Initialize all non-persistent global variables.
  */

void init_global_vars()  /*  RM: Feb 15 1993  */

{
  ptr_definition def;

  /* printf("initializing global vars...\n"); */
  
  /*
    for(def=first_definition;def;def=def->next) {
    if(def->type==global && ((GENERIC)def->global_value<heap_pointer)) {
    clear_copy();
    def->global_value=eval_copy(def->init_value,STACK); 
    }
    }
    */

  for(def=first_definition;def;def=def->next)
    if((GENERIC)(def->global_value)<(GENERIC)heap_pointer)
      def->global_value=NULL;
}

/********************************************************************/
