// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2016 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
#include <test_config.h>
#include <signal.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <GetOpt.h>


#include "TempFile.h"
#include "BESHandlerUtil.h"
#include "BESInternalError.h"
#include "BESInternalFatalError.h"
#include "TheBESKeys.h"


#include <debug.h>

static bool debug = false;
static bool debug_2 = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);
#undef DBG2
#define DBG2(x) do { if (debug_2) (x); } while(false);

using namespace CppUnit;
using namespace std;

string temp_dir = string(TEST_SRC_DIR) + "/tmp_dir";
string tmp_template = temp_dir + "/tmp_XXXXXX";

class TemporaryFileTest: public CppUnit::TestFixture {
private:

public:
    TemporaryFileTest()
    {
    }

    ~TemporaryFileTest()
    {
    }

    void setUp()
    {
        DBG2(cerr <<  __func__ << "() - BEGIN" << endl);

        // Because TemporaryFile uses the BESLog macro ERROR we have to configure the BESKeys with the
        // BES config file name so that there is a log file name... Oy.
        string bes_conf = TEST_SRC_DIR;
        bes_conf.append("/bes.conf");
        TheBESKeys::ConfigFile = bes_conf;

        DBG(cerr <<  __func__ << "() - Temp file template is: '" << tmp_template << "'"  << endl);

        DBG2(cerr <<  __func__ << "() - END" << endl);
    }

    void tearDown()
    {
        DBG2(cerr <<  __func__ << "() - BEGIN" << endl);


        DBG2(cerr <<  __func__ << "() - END" << endl);
    }

    void normal_test()
    {
        std::string tmp_file_name;

        try {
            bes::TempFile tf(tmp_template);
            tmp_file_name = tf.get_name();
            DBG(cerr <<  __func__ << "() - Temp file is: '" << tmp_file_name << "' has been created. fd: "<< tf.get_fd()  << endl);

            // Is it really there? Just sayin'...
            struct stat buf;
            int statret = stat(tmp_file_name.c_str(), &buf);
            CPPUNIT_ASSERT(statret == 0);
        }
        catch (BESInternalError &bie) {
            DBG(cerr <<  __func__ << "() - Caught BESInternalError! Message: "<< bie.get_message()  << endl);
            CPPUNIT_ASSERT(false);
        }
        // The name should be set to something.
        CPPUNIT_ASSERT(tmp_file_name.length()>0);

        struct stat buf;
        int statret = stat(tmp_file_name.c_str(), &buf);
        // And the file should be gone because the class TemporaryFile went out of scope.
        CPPUNIT_ASSERT(statret != 0);
        DBG(cerr <<  __func__ << "() - Temp file '" << tmp_file_name << "' has been removed. (Temporary file out of scope)"  << endl);


    }

    void exception_test()
    {
        std::string tmp_file_name;

        try {
            bes::TempFile tf(tmp_template);
            tmp_file_name = tf.get_name();
            DBG(cerr <<  __func__ << "() - Temp file is: '" << tmp_file_name << "' has been created. fd: "<< tf.get_fd()  << endl);

            // Is it really there? Just sayin'...
            struct stat buf;
            int statret = stat(tmp_file_name.c_str(), &buf);
            CPPUNIT_ASSERT(statret == 0);

            throw BESInternalError("Throwing an exception to challenge the lifecycle...", __FILE__, __LINE__);


        }
        catch (BESInternalError &bie) {
            DBG(cerr <<  __func__ << "() - Caught BESInternalError (Expected)  Message: "<< bie.get_message()  << endl);
        }
        // The name should be set to something.
        CPPUNIT_ASSERT(tmp_file_name.length()>0);

        struct stat buf;
        int statret = stat(tmp_file_name.c_str(), &buf);
        // And the file should be gone because the class TemporaryFile went out of scope.
        CPPUNIT_ASSERT(statret != 0);
        DBG(cerr <<  __func__ << "() - Temp file '" << tmp_file_name << "' has been removed. (Temporary file out of scope)"  << endl);


    }

    static void register_sigpipe_handler()
    {
        struct sigaction act;
        sigemptyset(&act.sa_mask);
        sigaddset(&act.sa_mask, SIGPIPE);
        act.sa_flags = 0;

        act.sa_handler = bes::TempFile::delete_temp_files;
        if (sigaction(SIGPIPE, &act, 0)) {
            cerr << "Could not register a handler to catch SIGPIPE." << endl;
            exit(1);
        }
    }



    void sigpipe_test()
    {

        char *glob_name;
        int name_size = tmp_template.length() + 1;


        glob_name = (char *) mmap(NULL, name_size, PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        pid_t pid = fork();
        CPPUNIT_ASSERT(pid >= 0);

        if(pid){
            //parent
            sleep(1);
            kill(pid,SIGPIPE);
            sleep(1);
            DBG(cerr <<  __func__ << "() - CLIENT SHOULD BE DEAD. Temporary File Name: '" << glob_name << "'"<< endl);
            // Is it STILL there? Better not be...
            struct stat buf;
            int statret = stat(glob_name, &buf);
            CPPUNIT_ASSERT(statret != 0);
            DBG(cerr <<  __func__ << "() - Temporary File: '" << glob_name  << "' was successfully removed. woot."<< endl);

            munmap(glob_name, name_size);

        }
        else {
            //child

            register_sigpipe_handler();

            std::string tmp_file_name;

            try {
                bes::TempFile tf(tmp_template);
                tmp_file_name = tf.get_name();
                DBG(cerr <<  __func__ << "() - Temp file is: '" << tmp_file_name << "' has been created. fd: "<< tf.get_fd()  << endl);
                tmp_file_name.copy(glob_name, tmp_file_name.size(), 0);

                // Is it really there? Just sayin'...
                struct stat buf;
                int statret = stat(tmp_file_name.c_str(), &buf);
                CPPUNIT_ASSERT(statret == 0);
                sleep(100);
                DBG(cerr <<  __func__ << "() - Client is Alive." << endl);
            }
            catch (BESInternalError &bie) {
                DBG(cerr <<  __func__ << "() - Caught BESInternalError  Message: "<< bie.get_message()  << endl);
                CPPUNIT_ASSERT(false);
            }
            DBG(cerr <<  __func__ << "() - Client is exiting." << endl);

        }


    }



    CPPUNIT_TEST_SUITE( TemporaryFileTest );

    CPPUNIT_TEST(normal_test);
    CPPUNIT_TEST(exception_test);
    CPPUNIT_TEST(sigpipe_test);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TemporaryFileTest);

int main(int argc, char*argv[])
{

    GetOpt getopt(argc, argv, "dDh");
    int option_char;
    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;
        case 'D':
            debug_2 = 1;
            break;
        case 'h': {     // help - show test names
            cerr << "Usage: TemporaryFileTest has the following tests:" << endl;
            const std::vector<Test*> &tests = TemporaryFileTest::suite()->getTests();
            unsigned int prefix_len = TemporaryFileTest::suite()->getName().append("::").length();
            for (std::vector<Test*>::const_iterator i = tests.begin(), e = tests.end(); i != e; ++i) {
                cerr << (*i)->getName().replace(0, prefix_len, "") << endl;
            }
            break;
        }
        default:
            break;
        }

    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    bool wasSuccessful = true;
    string test = "";
    int i = getopt.optind;
    if (i == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        while (i < argc) {
            if (debug) cerr << "Running " << argv[i] << endl;
            test = TemporaryFileTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
            i++;
        }
    }

    return wasSuccessful ? 0 : 1;
}

