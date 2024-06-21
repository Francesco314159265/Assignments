


#ifndef ALGEBRA_H
#define ALGEBRA_H




void multiply_matrix_vec22(double dest[2] , const double const matrix[2][2] , const double const vec[2]){
    dest[0] = matrix[0][0] * vec[0] + matrix[0][1] * vec[1];
    dest[1] = matrix[1][0] * vec[0] + matrix[1][1] * vec[1];
}


void add_matrix22_implace(double matrix1[2][2] , const double const matrix2[2][2]){
    matrix1[0][0] += matrix2[0][0];
    matrix1[0][1] += matrix2[0][1];
    matrix1[1][0] += matrix2[1][0];
    matrix1[1][1] += matrix2[1][1];

}



void scalar_multiply_matrix_implace(double s, double matrix[2][2]){
    matrix[0][0] *= s;
    matrix[0][1] *= s;
    matrix[1][0] *= s;
    matrix[1][1] *= s;
}


void outer_vec22(double dest[2][2] , const double const vec1[2] , const double const vec2[2]){
    dest[0][0] = vec1[0] * vec2[0];
    dest[1][0] = vec1[1] * vec2[0];
    dest[0][1] = vec1[0] * vec2[1];
    dest[1][1] = vec1[1] * vec2[1];
}

void inverse_mat22(double dest[2][2] , const double const matrix[2][2]){
    double det = matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];

    dest[0][0] = 1 / det * matrix[1][1];
    dest[1][1] = 1 / det * matrix[0][0];
    dest[0][1] = 1 / det * ( - matrix[0][1]);
    dest[1][0] = 1 / det *( - matrix[1][0]);

}


void sub_vec2_into_first(double v1[2] , const double const v2[2]){
    v1[0] -= v2[0];
    v1[1] -= v2[1]; 
}





void multiply_scalar_impl(double s , double vec[2]){
    vec[0] *= s;
    vec[1] *= s;
}

void add_into_vec(double vec1[2] , double vec2[2]){
    vec1[0] += vec2[0];
    vec1[1] += vec2[1];
}






#endif