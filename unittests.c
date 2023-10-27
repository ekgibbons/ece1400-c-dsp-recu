#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tests/utest.h"

#include "filters.h"
#include "mtxio.h"

#define TEST_LEN 4

double norm_difference(double *x, double *x_sol, int size)
{
    double diff = 0;
    for (int n = 0; n < size; n++)
    {
	diff += pow(x[n] - x_sol[n], 2);
    }

    return sqrt(diff);
}


UTEST(mtxio, read_1d_mtx)
{

    double b_sol[] = {2, 29, 12, 6};
    
    char filename[] = "tests/b_io.mtx";
    double b[TEST_LEN];

    int size_1d = read_1d_mtx(filename, b);

    ASSERT_EQ(size_1d, 4);


    double norm = norm_difference(b, b_sol, TEST_LEN);

    ASSERT_LT(norm, 10e-6);
    
}


UTEST(mtxio, write_1d_mtx)
{
    char filename_in[] = "tests/b_io.mtx";
    double b[TEST_LEN];
    int size_1d = read_1d_mtx(filename_in, b);

    char filename_out[] = "tests/b_tmp.mtx";
    write_1d_mtx(filename_out, b, size_1d);

    char command[50];
    sprintf(command, "diff -w %s %s ",filename_in, filename_out);
    int ret = system(command);
    ASSERT_TRUE(ret==0);
    
    sprintf(command, "rm %s",filename_out);
    ret = system(command);
    (void)ret;
}


UTEST(filters, sum)
{
    char command[] = "python -c 'import tests.python_filters; "
        "tests.python_filters.sum_test()' > tmp.txt";

    int ret = system(command);
    char filename_in[] = "x_test.mtx";

    double x[LEN];
    int size = read_1d_mtx(filename_in, x);

    double sum_sub = sum_wrap(x, size);

    FILE *sum_file = fopen("tmp.txt","r");
    double sum_sol;
    
    if (fscanf(sum_file,"%lf",&sum_sol) == -1)
    {
        printf("ERROR: failed to read determinant file\n");
        exit(1);
    }
    
    fclose(sum_file);
  
    double diff = fabs((sum_sol - sum_sub)/sum_sol);
    ASSERT_LT(diff, 10e-12);

    ret = system("rm tmp.txt x_test.mtx");
    (void)ret;
    
}


UTEST(filters, accumulator)
{
    char command[] = "python -c 'import tests.python_filters; "
        "tests.python_filters.accumulator_test()'";

    int ret = system(command);
    char filename_x[] = "x_test.mtx";
    char filename_y[] = "y_test.mtx";

    double x[LEN];
    double y_sol[LEN];
    double y_sub[LEN];
    
    int size = read_1d_mtx(filename_x, x);
    size = read_1d_mtx(filename_y, y_sol);

    accumulator_wrap(y_sub, x, size);

    double norm = norm_difference(y_sub, y_sol, size);

    ASSERT_LT(norm, 10e-12);

    ret = system("rm y_test.mtx x_test.mtx");
    (void)ret;
    
}


UTEST(filters, average_filter)
{
    char command[] = "python -c 'import tests.python_filters; "
        "tests.python_filters.mean_filter_test()' > tmp.txt";

    int ret = system(command);
    char filename_x[] = "x_test.mtx";
    char filename_y[] = "y_test.mtx";

    double x[LEN];
    double y_sol[LEN];
    double y_sub[LEN];
    
    int size = read_1d_mtx(filename_x, x);
    size = read_1d_mtx(filename_y, y_sol);

    FILE *len_file = fopen("tmp.txt","r");
    int filt_length;
    
    if (fscanf(len_file,"%i",&filt_length) == -1)
    {
        printf("ERROR: failed to read determinant file\n");
        exit(1);
    }
    
    fclose(len_file);

    average_filt_wrap(y_sub, x, filt_length, size);

    double norm = norm_difference(y_sub, y_sol, size);

    ASSERT_LT(norm, 10e-12);

    ret = system("rm y_test.mtx x_test.mtx tmp.txt");
    (void)ret;
    
}


UTEST(filters, looping_check)
{
    int out = system("grep 'for' filters.c");
    ASSERT_NE(out, 0);

    out = system("grep 'while' filters.c");
    ASSERT_NE(out, 0);

}

UTEST(main, usage)
{
    int out = system("./filter > tmp_1.txt");

    ASSERT_EQ(out, 0);

    FILE *fp = fopen("tmp_2.txt","w");

    fprintf(fp,"Usage:\n    $ ./filter <type> <in_file> <out_file>\n");
    fclose(fp);

    out = system("diff -w tmp_1.txt tmp_2.txt");

    ASSERT_EQ(out, 0);

    int ret = system("rm tmp_1.txt tmp_2.txt");
    (void)ret;

}


UTEST(main, usage_type)
{
    char type_use[] = "something";
    char command[100];

    sprintf(command,"./filter %s tmp.mtx tmp.mtx > tmp_1.txt",type_use);
    int out = system(command);

    ASSERT_EQ(out, 0);

    FILE *fp = fopen("tmp_2.txt","w");

    fprintf(fp,"Usage:\n    $ ./filter <type> <in_file> <out_file>\n");
    fprintf(fp,"%s invalid type\n",type_use);
    fprintf(fp,"must be 'average_filter', 'accumulator', or 'sum'\n");
    fclose(fp);

    out = system("diff -w tmp_1.txt tmp_2.txt");

    ASSERT_EQ(out, 0);

    int ret = system("rm tmp_1.txt tmp_2.txt");
    (void)ret;

}


UTEST(main, usage_out_file_accumulator)
{
    char type_use[] = "accumulator";
    char command[100];

    sprintf(command,"./filter %s tmp.mtx > tmp_1.txt",type_use);
    int out = system(command);

    ASSERT_EQ(out, 0);

    FILE *fp = fopen("tmp_2.txt","w");

    fprintf(fp,"Usage:\n    $ ./filter <type> <in_file> <out_file>\n");
    fprintf(fp,"must include <out_file> for '%s'\n",type_use);
    fclose(fp);

    out = system("diff -w tmp_1.txt tmp_2.txt");

    ASSERT_EQ(out, 0);

    int ret = system("rm tmp_1.txt tmp_2.txt");
    (void)ret;
}


UTEST(main, usage_out_file_average_filter)
{
    char type_use[] = "average_filter";
    char command[100];

    sprintf(command,"./filter %s tmp.mtx > tmp_1.txt",type_use);
    int out = system(command);

    ASSERT_EQ(out, 0);

    FILE *fp = fopen("tmp_2.txt","w");

    fprintf(fp,"Usage:\n    $ ./filter <type> <in_file> <out_file>\n");
    fprintf(fp,"must include <out_file> for '%s'\n",type_use);
    fclose(fp);

    out = system("diff -w tmp_1.txt tmp_2.txt");

    ASSERT_EQ(out, 0);

    int ret = system("rm tmp_1.txt tmp_2.txt");
    (void)ret;
}


UTEST(main, sum)
{
    char command[] = "python -c 'import tests.python_filters; "
        "tests.python_filters.sum_test()' > tmp.txt";

    int ret = system(command);

    FILE *sum_file = fopen("tmp.txt","r");
    double sum_sol;
    
    if (fscanf(sum_file,"%lf",&sum_sol) == -1)
    {
        printf("ERROR: failed to read determinant file\n");
        exit(1);
    }
    
    fclose(sum_file);

    FILE *fp = fopen("sol.txt","w");
    fprintf(fp, "the summation is %0.4e\n",sum_sol);
    fclose(fp);

    ret = system("./filter sum x_test.mtx > sub.txt");
    ASSERT_EQ(ret, 0);
        
    ret = system("diff -w sub.txt sol.txt");

    ASSERT_EQ(ret, 0);

    ret = system("rm tmp.txt sub.txt sol.txt x_test.mtx");
    (void)ret;
}


UTEST(main, accumulator)
{
    char command[] = "python -c 'import tests.python_filters; "
        "tests.python_filters.accumulator_test()'";

    int ret = system(command);

    FILE *fp = fopen("sol.txt","w");
    fprintf(fp, "accumulator processing successful\n");
    fclose(fp);

    ret = system("./filter accumulator x_test.mtx out_test.mtx > sub.txt");
    ASSERT_EQ(ret, 0);
        
    ret = system("diff -w sub.txt sol.txt");
    ASSERT_EQ(ret, 0);

    ret = system("diff -w y_test.mtx out_test.mtx > out.txt");
    ASSERT_EQ(ret, 0);

    ret = system("rm x_test.mtx y_test.mtx "
                 "out_test.mtx sub.txt sol.txt out.txt");
    (void)ret;
}


UTEST(main, average_filter)
{
    char command[] = "python -c 'import tests.python_filters; "
        "tests.python_filters.mean_filter_test(filt_length=10)' > tmp.txt";

    int ret = system(command);

    FILE *fp = fopen("sol.txt","w");
    fprintf(fp, "average_filter processing successful\n");
    fclose(fp);

    ret = system("./filter average_filter x_test.mtx out_test.mtx > sub.txt");
    ASSERT_EQ(ret, 0);
        
    ret = system("diff -w sub.txt sol.txt > tmp.txt");
    ASSERT_EQ(ret, 0);

    double y_sol[LEN];
    double y_sub[LEN];
    
    int size = read_1d_mtx("out_test.mtx", y_sub);
    size = read_1d_mtx("y_test.mtx", y_sol);

    double norm = norm_difference(y_sub, y_sol, size);

    ASSERT_LT(norm, 10e-12);

    ret = system("rm x_test.mtx y_test.mtx "
                 "out_test.mtx sub.txt sol.txt tmp.txt");    
    (void)ret;
}




UTEST_MAIN();
