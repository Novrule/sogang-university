/**************************************************
 * Title: DatabaseSystem - Project 2
 * Summary: 
 *  |Date              |Author             |Version
    |2024-06-02        |Wonjun Han         |1.0.0
**************************************************/
#include <stdio.h>
#include "mysql.h"

#pragma comment(lib, "libmysql.lib")

const char* host = "localhost";
const char* user = "root";
const char* pw = "mysql";
const char* db = "project";

#define MAX_LEN 5000

int main(void) {
    MYSQL* connection = NULL;
    MYSQL conn;
    MYSQL_RES* sql_result;
    MYSQL_ROW sql_row;
    FILE* fp = fopen("CRUD.txt", "rt"); // open CRUD file.
    char line[MAX_LEN];

    if (mysql_init(&conn) == NULL)
        printf("mysql_init() error!");

    connection = mysql_real_connect(&conn, host, user, pw, NULL, 3306, (const char*)NULL, 0); // the first NULL can be replaced to an existing db instance.
    if (connection == NULL) { // MySQL과의 연결에 실패한 경우
        printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

        return 1;
    }
    else { // MySQL과의 연결에 성공한 경우
        while (fgets(line, sizeof(line), fp) != NULL) {
            if (!strcmp(line, "$$$\n")) { // read lines from CRUD file, '$$$' separates CREATE / DELETE parts.
                break;
            }

            mysql_query(connection, line);
        }

        if (mysql_select_db(&conn, "project")) { // DB를 선택하는데 실패한 경우
            printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

            return 1;
        }
        else { // DB를 선택하는데 성공한 경우
            printf("Connection Succeed\n\n");

            while (1) { // 프로그램이 종료될 때까지 반복
                int type = 0; // 명령의 타입을 저장하는 변수
                char temp; // 오류 메시지를 없애기 위해 선언한 임시 변수

                printf("------- SELECT QUERY TYPES -------\n\n");
                printf("\t1. TYPE 1\n");
                printf("\t2. TYPE 2\n");
                printf("\t3. TYPE 3\n");
                printf("\t4. TYPE 4\n");
                printf("\t5. TYPE 5\n");
                printf("\t6. TYPE 6\n");
                printf("\t7. TYPE 7\n");
                printf("\t0. QUIT\n\n");
                printf("----------------------------------\n\n");

                printf("Select Number: ");
                temp = scanf("%d", &type); // 타입을 입력 받음
                temp = getchar();

                if (type == 1) {
                    int state = 0;
                    int subtype = 0;
                    char query[MAX_LEN];

                    printf("\n");
                    printf("------------- TYPE 1 -------------\n\n"); // 입력 받은 명령이 타입 1인 경우

                    sprintf(query, "SELECT property_name, street_name, street_number FROM properties WHERE district = 'Mapo' AND sale_status = 0;"); // 타입 1에 맞는 SQL 쿼리를 보냄

                    state = mysql_query(connection, query);

                    if (state == 0) {
                        sql_result = mysql_store_result(connection);

                        while ((sql_row = mysql_fetch_row(sql_result)) != NULL) { // SQL 쿼리에 대한 응답을 출력
                            printf("property name : %s, street name : %s, street number : %s\n", sql_row[0], sql_row[1], sql_row[2]);
                        }
                        printf("\n");

                        mysql_free_result(sql_result);

                        printf("------- SUBTYPES IN TYPE 1 -------\n\n");
                        printf("\t1. TYPE 1-1\n");
                        printf("\t0. QUIT\n\n");
                        printf("----------------------------------\n\n");

                        printf("Select Number: ");
                        temp = scanf("%d", &subtype);
                        temp = getchar();

                        if (subtype == 1) {
                            printf("\n");
                            printf("------------ TYPE 1-1 ------------\n\n");  // 입력 받은 명령이 타입 1-1인 경우

                            sprintf(query, "SELECT property_name, street_name, street_number FROM properties WHERE district = 'Mapo' AND sale_status = 0 AND price BETWEEN 1000000000 AND 1500000000;");  // 타입 1-1에 맞는 SQL 쿼리를 보냄

							state = mysql_query(connection, query);

							if (state == 0) {
								sql_result = mysql_store_result(connection);

								while ((sql_row = mysql_fetch_row(sql_result)) != NULL) { // SQL 쿼리에 대한 응답을 출력
									printf("property name : %s, street name : %s, street number : %s\n", sql_row[0], sql_row[1], sql_row[2]);
								}
								printf("\n");

								mysql_free_result(sql_result);
							}
							else {
								printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                                return 1;
							}
						}
						else if (subtype == 0) {
                            continue;
						}
                    }
                    else {
                        printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                        return 1;
                    }
                }
                else if (type == 2) { // 입력 받은 명령이 타입1인 경우
                    int state = 0;
                    int subtype = 0;
                    char query[MAX_LEN];

                    printf("\n");
                    printf("------------- TYPE 2 -------------\n\n"); // 입력 받은 명령이 타입 2인 경우

                    sprintf(query, "SELECT p.property_name, p.street_name, p.street_number FROM properties p JOIN district_info d ON p.district = d.district WHERE d.school_district = '8th' AND p.sale_status = 0;"); // 타입 2에 맞는 SQL 쿼리를 보냄

                    state = mysql_query(connection, query);

                    if (state == 0) {
                        sql_result = mysql_store_result(connection);

                        while ((sql_row = mysql_fetch_row(sql_result)) != NULL) { // SQL 쿼리에 대한 응답을 출력
                            printf("property name : %s, street name : %s, street number : %s\n", sql_row[0], sql_row[1], sql_row[2]);
                        }
                        printf("\n");

                        mysql_free_result(sql_result);


                        printf("------- SUBTYPES IN TYPE 2 -------\n\n");
                        printf("\t1. TYPE 2-1\n");
                        printf("\t0. QUIT\n\n");
                        printf("----------------------------------\n\n");

                        printf("Select Number: ");
                        temp = scanf("%d", &subtype);
                        temp = getchar();

                        if (subtype == 1) {
                            printf("\n");
                            printf("------------ TYPE 2-1 ------------\n\n"); // 입력 받은 명령이 타입 2-1인 경우

                            sprintf(query, "SELECT p.property_name, p.street_name, p.street_number FROM properties p JOIN district_info d ON p.district = d.district WHERE d.school_district = '8th' AND p.sale_status = 0 AND p.bed_room_count >= 4 AND p.bath_room_count >= 2;"); // 타입 2-1에 맞는 SQL 쿼리를 보냄

                            state = mysql_query(connection, query);

                            if (state == 0) {
                                sql_result = mysql_store_result(connection);

                                while ((sql_row = mysql_fetch_row(sql_result)) != NULL) { // SQL 쿼리에 대한 응답을 출력
                                    printf("property name : %s, street name : %s, street number : %s\n", sql_row[0], sql_row[1], sql_row[2]);
                                }
                                printf("\n");

                                mysql_free_result(sql_result);
                            }
                            else {
                                printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                                return 1;
                            }
                        }
                        else if (subtype == 0) {
                            continue;
                        }
                    }
                    else {
                        printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                        return 1;
                    }
                }
                else if (type == 3) {
                    int state = 0;
                    int subtype = 0;
                    char query[MAX_LEN];

                    printf("\n");
                    printf("------------- TYPE 3 -------------\n\n"); // 입력 받은 명령이 타입 3인 경우

                    sprintf(query, "SELECT a.agent_name, SUM(p.price) FROM agents a JOIN manage_property mp ON a.agent_id = mp.agent_id JOIN buy_property bp ON mp.property_id = bp.property_id JOIN properties p ON bp.property_id = p.property_id WHERE YEAR(bp.buy_date) = 2022 AND mp.agent_type = 1 GROUP BY a.agent_name ORDER BY SUM(p.price) DESC LIMIT 1;"); // 타입 3에 맞는 SQL 쿼리를 보냄

                    state = mysql_query(connection, query);

                    if (state == 0) {
                        sql_result = mysql_store_result(connection);

                        while ((sql_row = mysql_fetch_row(sql_result)) != NULL) { // SQL 쿼리에 대한 응답을 출력
                            printf("agent name : %s, total price : %s won\n", sql_row[0], sql_row[1]);
                        }
                        printf("\n");

                        mysql_free_result(sql_result);


                        printf("------- SUBTYPES IN TYPE 3 -------\n\n");
                        printf("\t1. TYPE 3-1\n");
                        printf("\t2. TYPE 3-2\n");
                        printf("\t0. QUIT\n\n");
                        printf("----------------------------------\n\n");

                        printf("Select Number: ");
                        temp = scanf("%d", &subtype);
                        temp = getchar();

                        if (subtype == 1) {
                            char k[10];

                            printf("\n");
                            printf("------------ TYPE 3-1 ------------\n\n"); // 입력 받은 명령이 타입 3-1인 경우

                            printf("**Enter the k to be tracked**\n");
                            printf("Which k? : ");
                            temp = scanf("%s", k);
                            temp = getchar();

                            sprintf(query, "SELECT a.agent_name, COALESCE(SUM(p.price), 0) FROM agents a LEFT JOIN manage_property mp ON a.agent_id = mp.agent_id LEFT JOIN buy_property bp ON mp.property_id = bp.property_id AND YEAR(bp.buy_date) = 2023 LEFT JOIN properties p ON bp.property_id = p.property_id WHERE mp.agent_type = 1 GROUP BY a.agent_name ORDER BY COALESCE(SUM(p.price), 0) DESC LIMIT %s;", k); // 타입 3-1에 맞는 SQL 쿼리를 보냄

                            state = mysql_query(connection, query);

                            if (state == 0) {
                                sql_result = mysql_store_result(connection);

                                printf("\n");
                                while ((sql_row = mysql_fetch_row(sql_result)) != NULL) { // SQL 쿼리에 대한 응답을 출력
                                    printf("agent name : %s, total price : %s won\n", sql_row[0], sql_row[1]);
                                }
                                printf("\n");

                                mysql_free_result(sql_result);
                            }
                            else {
                                printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                                return 1;
                            }
                        }
                        if (subtype == 2) {
                            printf("\n");
                            printf("------------ TYPE 3-2 ------------\n\n"); // 입력 받은 명령이 타입 3-2인 경우

                            sprintf(query, "SELECT agent_name, total_value FROM (SELECT a.agent_id, a.agent_name, COALESCE(SUM(p.price), 0) AS total_value, RANK() OVER (ORDER BY COALESCE(SUM(p.price), 0)) AS ranking FROM agents a LEFT JOIN manage_property mp ON a.agent_id = mp.agent_id LEFT JOIN buy_property bp ON mp.property_id = bp.property_id AND YEAR(bp.buy_date) = 2021 LEFT JOIN properties p ON bp.property_id = p.property_id WHERE mp.agent_type = 1 GROUP BY a.agent_id, a.agent_name) AS ranked_agents WHERE ranking <= CEIL((SELECT COUNT(*) FROM (SELECT a.agent_id FROM agents a LEFT JOIN manage_property mp ON a.agent_id = mp.agent_id LEFT JOIN buy_property bp ON mp.property_id = bp.property_id AND YEAR(bp.buy_date) = 2021 LEFT JOIN properties p ON bp.property_id = p.property_id GROUP BY a.agent_id) AS total_agents) * 0.1);"); // 타입 3-2에 맞는 SQL 쿼리를 보냄

                            state = mysql_query(connection, query);

                            if (state == 0) {
                                sql_result = mysql_store_result(connection);

                                while ((sql_row = mysql_fetch_row(sql_result)) != NULL) { // SQL 쿼리에 대한 응답을 출력
                                    printf("agent name : %s, total price : %s won\n", sql_row[0], sql_row[1]);
                                }
                                printf("\n");

                                mysql_free_result(sql_result);
                            }
                            else {
                                printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                                return 1;
                            }
                        }
                        else if (subtype == 0) {
                            continue;
                        }
                    }
                    else {
                        printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                        return 1;
                    }
                }
                else if (type == 4) {
                    int state = 0;
                    int subtype = 0;
                    char query[MAX_LEN];

                    printf("\n");
                    printf("------------- TYPE 4 -------------\n\n"); // 입력 받은 명령이 타입 4인 경우

                    sprintf(query, "SELECT a.agent_name, COALESCE(ROUND(AVG(p.price)), 0), COALESCE(ROUND(AVG(DATEDIFF(bp.buy_date, p.construction_date))), 0) FROM agents a LEFT JOIN manage_property mp ON a.agent_id = mp.agent_id LEFT JOIN buy_property bp ON mp.property_id = bp.property_id AND YEAR(bp.buy_date) = 2022 LEFT JOIN properties p ON bp.property_id = p.property_id WHERE mp.agent_type = 1 GROUP BY a.agent_id, a.agent_name;"); // 타입 4에 맞는 SQL 쿼리를 보냄

                    state = mysql_query(connection, query);

                    if (state == 0) {
                        sql_result = mysql_store_result(connection);

                        while ((sql_row = mysql_fetch_row(sql_result)) != NULL) { // SQL 쿼리에 대한 응답을 출력
                            printf("agent name : %s, average price : %s won, average time : %s day\n", sql_row[0], sql_row[1], sql_row[2]);
                        }
                        printf("\n");

                        mysql_free_result(sql_result);

                        printf("------- SUBTYPES IN TYPE 4 -------\n\n");
                        printf("\t1. TYPE 4-1\n");
                        printf("\t2. TYPE 4-2\n");
                        printf("\t0. QUIT\n\n");
                        printf("----------------------------------\n\n");

                        printf("Select Number: ");
                        temp = scanf("%d", &subtype);
                        temp = getchar();

                        if (subtype == 1) {
                            printf("\n");
                            printf("------------ TYPE 4-1 ------------\n\n"); // 입력 받은 명령이 타입 4-1인 경우

                            sprintf(query, "SELECT a.agent_name, COALESCE(MAX(p.price), 0) FROM agents a LEFT JOIN manage_property mp ON a.agent_id = mp.agent_id LEFT JOIN buy_property bp ON mp.property_id = bp.property_id AND YEAR(bp.buy_date) = 2023 LEFT JOIN properties p ON bp.property_id = p.property_id WHERE mp.agent_type = 1 GROUP BY a.agent_id, a.agent_name;"); // 타입 4-1에 맞는 SQL 쿼리를 보냄

                            state = mysql_query(connection, query);

                            if (state == 0) {
                                sql_result = mysql_store_result(connection);

                                while ((sql_row = mysql_fetch_row(sql_result)) != NULL) { // SQL 쿼리에 대한 응답을 출력
                                    printf("agent name : %s, max price : %s won\n", sql_row[0], sql_row[1]);
                                }
                                printf("\n");

                                mysql_free_result(sql_result);
                            }
                            else {
                                printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                                return 1;
                            }
                        }
                        if (subtype == 2) {
                            printf("\n");
                            printf("------------ TYPE 4-2 ------------\n\n"); // 입력 받은 명령이 타입 4-2인 경우

                            sprintf(query, "SELECT a.agent_name, COALESCE(MAX(DATEDIFF(bp.buy_date, p.construction_date)), 0) FROM agents a LEFT JOIN manage_property mp ON a.agent_id = mp.agent_id LEFT JOIN buy_property bp ON mp.property_id = bp.property_id LEFT JOIN properties p ON bp.property_id = p.property_id WHERE mp.agent_type = 1 GROUP BY a.agent_id, a.agent_name;"); // 타입 4-2에 맞는 SQL 쿼리를 보냄

                            state = mysql_query(connection, query);

                            if (state == 0) {
                                sql_result = mysql_store_result(connection);

                                while ((sql_row = mysql_fetch_row(sql_result)) != NULL) { // SQL 쿼리에 대한 응답을 출력
                                    printf("agent name : %s, longest time : %s day\n", sql_row[0], sql_row[1]);
                                }
                                printf("\n");

                                mysql_free_result(sql_result);
                            }
                            else {
                                printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                                return 1;
                            }
                        }
                        else if (subtype == 0) {
                            continue;
                        }
                    }
                    else {
                        printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                        return 1;
                    }
                }
                if (type == 5) {
                    int state = 0;
                    int subtype = 0;
                    char query[MAX_LEN];

                    printf("\n");
                    printf("------------- TYPE 5 -------------\n\n"); // 입력 받은 명령이 타입 5인 경우

                    sprintf(query, "(SELECT p.property_name, p.property_type, ph.photo_name, photo_file FROM photos ph JOIN properties p ON ph.property_id = p.property_id WHERE p.property_type = 'studio' ORDER BY p.price DESC LIMIT 1) UNION ALL (SELECT p.property_name, p.property_type, ph.photo_name, photo_file FROM photos ph JOIN properties p ON ph.property_id = p.property_id WHERE p.property_type = 'one-bedroom' ORDER BY p.price DESC LIMIT 1) UNION ALL (SELECT p.property_name, p.property_type, ph.photo_name, photo_file FROM photos ph JOIN properties p ON ph.property_id = p.property_id WHERE p.property_type = 'multi-bedroom' ORDER BY p.price DESC LIMIT 1) UNION ALL (SELECT p.property_name, p.property_type, ph.photo_name, photo_file FROM photos ph JOIN properties p ON ph.property_id = p.property_id WHERE p.property_type = 'detached' ORDER BY p.price DESC LIMIT 1);"); // 타입 5에 맞는 SQL 쿼리를 보냄

                    state = mysql_query(connection, query);

                    if (state == 0) {
                        sql_result = mysql_store_result(connection);

                        while ((sql_row = mysql_fetch_row(sql_result)) != NULL) { // SQL 쿼리에 대한 응답을 출력
                            printf("property name : %s, property type : %s, photo name : %s, photo file : %s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3]);
                        }
                        printf("\n");

                        mysql_free_result(sql_result);
                    }
                    else {
                        printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                        return 1;
                    }
                }
                if (type == 6) {
                    int state = 0;
                    int subtype = 0;
                    char query[MAX_LEN];
                    char property_id[20];
                    char price[20];
                    char buyer_id[20];
                    char buyer_name[20];
                    char seller_id[20];
                    char seller_name[20];
                    char buyer_agent_id[20];
                    char buyer_agent_name[20];
                    char seller_agent_id[20];
                    char seller_agent_name[20];
                    char buy_date[20];

                    printf("\n");
                    printf("------------- TYPE 6 -------------\n\n"); // 입력 받은 명령이 타입 6인 경우

                    printf("**Enter the property id to be added**\n");
                    printf("Which property id? : ");
                    temp = scanf("%s", property_id);
                    temp = getchar();

                    printf("\n");
                    printf("**Enter the price to be added**\n");
                    printf("Which price? : ");
                    temp = scanf("%s", price);
                    temp = getchar();

                    printf("\n");
                    printf("**Enter the buyer id to be added**\n");
                    printf("Which buyer id? : ");
                    temp = scanf("%s", buyer_id);
                    temp = getchar();

                    sprintf(query, "SELECT COUNT(*) FROM buyers WHERE buyer_id = '%s';", buyer_id);

                    state = mysql_query(connection, query);

                    if (state == 0) {
                        sql_result = mysql_store_result(connection);

                        sql_row = mysql_fetch_row(sql_result); // SQL 쿼리에 대한 응답을 수신
                        
                        if (sql_row[0][0] == '0') { // 해당하는 buyer_id가 없는 경우
                            printf("\n");
                            printf("**Enter the buyer name to be added**\n");
                            printf("Which buyer name? : ");
                            temp = scanf("%s", buyer_name);
                            temp = getchar();

                            sprintf(query, "INSERT INTO buyers (buyer_id, buyer_name) VALUES ('%s', '%s');", buyer_id, buyer_name); // buyer id와 buyer name을 buyers 테이블에 추가

                            state = mysql_query(connection, query);

                            if (state != 0) {
                                printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                                return 1;
                            }
                        }

                        mysql_free_result(sql_result);
                    }
                    else {
                        printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                        return 1;
                    }

                    printf("\n");
                    printf("**Enter the seller id to be added**\n");
                    printf("Which seller id? : ");
                    temp = scanf("%s", seller_id);
                    temp = getchar();

                    sprintf(query, "SELECT COUNT(*) FROM sellers WHERE seller_id = '%s';", seller_id);

                    state = mysql_query(connection, query);

                    if (state == 0) {
                        sql_result = mysql_store_result(connection);

                        sql_row = mysql_fetch_row(sql_result); // SQL 쿼리에 대한 응답을 수신

                        if (sql_row[0][0] == '0') { // 해당하는 seller_id가 없는 경우
                            printf("\n");
                            printf("**Enter the seller name to be added**\n");
                            printf("Which seller name? : ");
                            temp = scanf("%s", seller_name);
                            temp = getchar();

                            sprintf(query, "INSERT INTO sellers (seller_id, seller_name) VALUES ('%s', '%s');", seller_id, seller_name); // seller id와 seller name을 sellers 테이블에 추가

                            state = mysql_query(connection, query);

                            if (state != 0) {
                                printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                                return 1;
                            }
                        }

                        mysql_free_result(sql_result);
                    }
                    else {
                        printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                        return 1;
                    }

                    printf("\n");
                    printf("**Enter the buyer agent id to be added**\n");
                    printf("Which buyer agent id? (If it's not there, type 'none'.) : ");
                    temp = scanf("%s", buyer_agent_id);
                    temp = getchar();

                    if (buyer_agent_id[0] != 'n' || buyer_agent_id[1] != 'o' || buyer_agent_id[2] != 'n' || buyer_agent_id[3] != 'e') {
                        sprintf(query, "SELECT COUNT(*) FROM agents WHERE agent_id = '%s';", buyer_agent_id);

                        state = mysql_query(connection, query);

                        if (state == 0) {
                            sql_result = mysql_store_result(connection);

                            sql_row = mysql_fetch_row(sql_result); // SQL 쿼리에 대한 응답을 수신

                            if (sql_row[0][0] == '0') { // 해당하는 buyer_agent_id가 없는 경우
                                printf("\n");
                                printf("**Enter the buyer agent name to be added**\n");
                                printf("Which buyer agent name? : ");
                                temp = scanf("%s", buyer_agent_name);
                                temp = getchar();

                                sprintf(query, "INSERT INTO agents (agent_id, agent_name) VALUES ('%s', '%s');", buyer_agent_id, buyer_agent_name); // buyer agent id와 buyer agent name을 agents 테이블에 추가

                                state = mysql_query(connection, query);

                                if (state != 0) {
                                    printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                                    return 1;
                                }
                            }

                            mysql_free_result(sql_result);
                        }
                        else {
                            printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                            return 1;
                        }
                    }

                    printf("\n");
                    printf("**Enter the seller agent id to be added**\n");
                    printf("Which seller agent id? (If it's not there, type 'none'.) : ");
                    temp = scanf("%s", seller_agent_id);
                    temp = getchar();

                    if (seller_agent_id[0] != 'n' || seller_agent_id[1] != 'o' || seller_agent_id[2] != 'n' || seller_agent_id[3] != 'e') {
                        sprintf(query, "SELECT COUNT(*) FROM agents WHERE agent_id = '%s';", seller_agent_id);

                        state = mysql_query(connection, query);

                        if (state == 0) {
                            sql_result = mysql_store_result(connection);

                            sql_row = mysql_fetch_row(sql_result); // SQL 쿼리에 대한 응답을 수신

                            if (sql_row[0][0] == '0') { // 해당하는 seller_agent_id가 없는 경우
                                printf("\n");
                                printf("**Enter the seller agent name to be added**\n");
                                printf("Which seller agent name? : ");
                                temp = scanf("%s", seller_agent_name);
                                temp = getchar();

                                sprintf(query, "INSERT INTO sellers (agent_id, agent_name) VALUES ('%s', '%s');", seller_agent_id, seller_agent_name); // seller agent id와 seller agent name을 agents 테이블에 추가

                                state = mysql_query(connection, query);

                                if (state != 0) {
                                    printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                                    return 1;
                                }
                            }

                            mysql_free_result(sql_result);
                        }
                        else {
                            printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                            return 1;
                        }
                    }

                    printf("\n");
                    printf("**Enter the buy date to be added**\n");
                    printf("Which buy date? (EX. 20XX-XX-XX) : ");
                    temp = scanf("%s", buy_date);
                    temp = getchar();

                    sprintf(query, "INSERT INTO properties (property_id, property_name, property_type, sale_type, sale_status, construction_date, price, area, district, street_name, street_number, living_room_count, bed_room_count, bath_room_count) VALUES ('%s', null, null, null, 1, null, %s, null, null, null, null, null, null, null);", property_id, price);

                    state = mysql_query(connection, query);

                    sprintf(query, "INSERT INTO buy_property (buyer_id, property_id, buy_date) VALUES ('%s', '%s', '%s');", buyer_id, property_id, buy_date);

                    state = mysql_query(connection, query);

                    sprintf(query, "INSERT INTO sell_property (seller_id, property_id) VALUES ('%s', '%s');", seller_id, property_id);

                    state = mysql_query(connection, query);

                    if (buyer_agent_id[0] != 'n' || buyer_agent_id[1] != 'o' || buyer_agent_id[2] != 'n' || buyer_agent_id[3] != 'e') {
                        sprintf(query, "INSERT INTO manage_property (agent_id, property_id, agent_type) VALUES ('%s', '%s', 0);", buyer_agent_id, property_id);

                        state = mysql_query(connection, query);
					}

                    if (seller_agent_id[0] != 'n' || seller_agent_id[1] != 'o' || seller_agent_id[2] != 'n' || seller_agent_id[3] != 'e') {
                        sprintf(query, "INSERT INTO manage_property (agent_id, property_id, agent_type) VALUES ('%s', '%s', 1);", seller_agent_id, property_id);

                        state = mysql_query(connection, query);
                    }

                    printf("\n");
                    printf("Property %s has been added.\n", property_id);
                    printf("\n");
                }
                if (type == 7) {
                    int state = 0;
                    int subtype = 0;
                    char query[MAX_LEN];
                    char agent_name[20];
                    char agent_id[20];

                    printf("\n");
                    printf("------------- TYPE 7 -------------\n\n"); // 입력 받은 명령이 타입 7인 경우

                    printf("**Enter the agent id to be added**\n");
                    printf("Which agent id? : ");
                    temp = scanf("%s", agent_id);
                    temp = getchar();

                    printf("\n");
                    printf("**Enter the agent name to be added**\n");
                    printf("Which agent name? : ");
                    temp = scanf("%s", agent_name);
                    temp = getchar();

                    sprintf(query, "INSERT INTO agents (agent_id, agent_name) VALUES ('%s', '%s');", agent_id, agent_name); // 타입 7에 맞는 SQL 쿼리를 보냄

                    state = mysql_query(connection, query);

                    if (state == 0) {
                        printf("\n");
                        printf("Agent %s has been added.\n", agent_name);
                        printf("\n");
                    }
                    else {
                        printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));

                        return 1;
                    }
                }
                else if (type == 0) { // 입력 받은 명령이 타입 0인 경우 (프로그램 종료)
                    break;
                }
            }
        }

        while (fgets(line, sizeof(line), fp) != NULL)
            mysql_query(connection, line);  // these are DELETEs & DROPs.

        mysql_close(connection);
    }

    return 0;
}
