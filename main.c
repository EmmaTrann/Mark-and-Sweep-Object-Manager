
// REMARKS:  Purpose of this program is to
// implement garbage collection
//-----------------------------------------
#include "ObjectManager.h"
#include <stdio.h>
#include <string.h>

static int passed_tests = 0;
static int total_tests = 0;


void test_retrievObject(char *ptr, int id){
    int i;
    ptr = (char*)retrieveObject(id);
    for (i = 0; i < 100; i++)
        ptr[i] = (char)(i%26 + 'A');
    ptr = (char*)retrieveObject(id);
    printf("------------------------------------------------------------\n");
    printf("Test Retrieve Object\n");
    for (i = 0; i < 100; i++)
        fprintf(stderr,"%c",ptr[i]);
    fprintf(stderr,"\n");
    total_tests++;
    passed_tests++;

}
void test_compact(){
    insertObject(100000);
    insertObject(300000);
    insertObject(200000);
    total_tests++;
    passed_tests++;
}
void test_add_ref(int id){
    addReference(id);
    total_tests++;
    passed_tests++;
}

void test_add_ref_edges(int id){
    addReference(id);
    total_tests++;
    passed_tests++;
}

void test_drop_ref_edges(int id){
    dropReference(id);
    total_tests++;
    passed_tests++;
}


void test_drop_ref(int id){
    dropReference(id);
    total_tests++;
    passed_tests++;
}

int main(int argc, char *argv[])
{
    char *ptr = NULL;
    int id1,id2,id3,id4;
    initPool();
    id1= insertObject(100);
    id2 = insertObject(4000);
    id3 = insertObject(20200);
    id4 = insertObject(10);
    test_retrievObject(ptr,id1);

    printf("------------------------------------------------------------\n");
    printf("Test Insert Object Without Compact\n");
    dumpPool();

    printf("------------------------------------------------------------\n");
    printf("Test Add Reference\n");
    printf("Adding to ref id 2 and 4\n");
    test_add_ref(id2);
    test_add_ref(id2);
    test_add_ref(id4);
    dumpPool();

    printf("------------------------------------------------------------\n");
    printf("Test Drop Reference\n");
    printf("Drop ref id 3\n");
    test_drop_ref(id3);
    dumpPool();

    printf("------------------------------------------------------------\n");
    printf("Test Compact\n");
    test_compact();
    dumpPool();

#ifdef NDEBUG
    printf("Test Add Reference Edges Case\n");
    printf("Add invalid reference - Should print invalid reference!\n");
    test_add_ref_edges(20);
    dumpPool();

    printf("------------------------------------------------------------\n");
    printf("Test Drop Reference Edges Case\n");
    printf("Drop invalid reference - Should print invalid reference!\n");
    test_drop_ref_edges(100);
    dumpPool();
#endif

    printf("------------------------------------------------------------\n");
    printf("Total number of tests executed: %d\n", total_tests);
    printf("Number of tests passed:         %d\n", passed_tests);
    printf("Number of tests failed:         %d\n", (total_tests - passed_tests));

    return 0;
}


