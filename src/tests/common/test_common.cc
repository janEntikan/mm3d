/*  Misfit Model 3D
 * 
 *  Copyright (c) 2008 Kevin Worcester
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
 *  USA.
 *
 *  See the COPYING file for full license text.
 */


// This file defines common functions used by tests in the libmm3d directory.

#include "test_common.h"
#include "mm3dfilter.h"
#include "test_common.h"

QString ConvertValToString( int val )
{
   return QString::number( val );
}

QString ConvertValToString( const std::string & val )
{
   return QString( val.c_str() );
}

void CompareValTrue(bool cond, const char * cond_text, const char * file, int line )
{
   if ( !cond )
   {
      QString msg = QString("'") + QString(cond_text);
      msg += "' returned FALSE (expected TRUE)";
      QTest::qFail( msg.toUtf8(), file, line );
   }
}

void CompareValFalse(bool cond, const char * cond_text, const char * file, int line )
{
   if ( !(!cond) )
   {
      QString msg = QString("'") + QString(cond_text);
      msg += "' returned TRUE (expected FALSE)";
      QTest::qFail( msg.toUtf8(), file, line );
   }
}

void model_status( Model * model, StatusTypeE type, unsigned ms, const char * fmt, ... )
{
   // FIXME hack
}

Model * loadModelOrDie( const char * filename )
{
   MisfitFilter f;

   Model * model = new Model;
   Model::ModelErrorE err = f.readFile( model, filename );

   if ( err != Model::ERROR_NONE )
   {
      fprintf( stderr, "fatal: %s: %s\n", filename, Model::errorToString( err ) );
      delete model;
      exit( -1 );
   }

   model->setUndoEnabled( true );
   model->forceAddOrDelete( true );
   return model;
}

Model * newTestModel()
{
   Model * model = new Model;
   model->setUndoEnabled( true );
   model->forceAddOrDelete( true );
   return model;
}

typedef std::vector<Model *> ModelList;

void checkUndoRedo( int operations, Model * lhs, const ModelList & rhs_list )
{
   // N operations, N+1 models in the list to compare against
   QVERIFY_EQ( (int) rhs_list.size(), operations + 1 );

   int bits = Model::CompareAll;
   QVERIFY_EQ( bits, lhs->equal( rhs_list.back(), bits ) );

   for ( int iter = 0; iter < 2; ++iter )
   {
      for ( int i = operations - 1; i >= 0; --i )
      {
         //printf( "undo operation %d\n", i );
         lhs->undo();
         QVERIFY_EQ( bits, lhs->equal( rhs_list[i], bits ) );
      }

      for ( int i = 1; i <= operations; ++i )
      {
         //printf( "redo operation %d\n", i );
         lhs->redo();
         QVERIFY_EQ( bits, lhs->equal( rhs_list[i], bits ) );
      }
   }
}

