#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#define I2C_ADDR 0x49
typedef unsigned char uint8;

static int read_HWclock(int fd, char buff[], int addr, int count)
{
    unsigned char ZorF; //正还是负
    int res;
    unsigned int tempeture = 0;
    float wendu;

    if(write(fd,&addr,1)!=1)        //写地址失败
    {
        //printf("write false \r\n");
        return -1;   
    }

    res=read(fd,buff,count);

    tempeture = buff[0];

    tempeture=tempeture<<8;
    tempeture=tempeture|buff[1];
    tempeture=tempeture>>6;

    if(buff[0]&0x80) //负
    {
        tempeture=tempeture-0x01;
        tempeture=~tempeture; //变回原码
        tempeture=tempeture&0x03FF;
        wendu = (float)((double)tempeture/4);
        ZorF=0;
    }
    else //正
    {
        wendu = (float)((double)tempeture/4);
        ZorF=1;
    }
    printf("%.2f",wendu);
    return res;
}

int main(void)
{
	time_t now;   
	struct tm *timenow;   
	char strtemp[255];   
	  
    int fd;
    char buf[3]; 
    char val,value;
    float flight;

    fd=open("/dev/i2c-1",O_RDWR);
    if(fd<0){
        printf("err open file:%s\r\n",strerror(errno)); return 1;
    }

    if(ioctl( fd,I2C_SLAVE,I2C_ADDR)<0 ){
        printf("ioctl error : %s\r\n",strerror(errno));return 1;
    }
    else{
        printf("ioctl ok \r\n");
    }
    int addr = 0;

    


	time(&now);   
	timenow = localtime(&now);   
	// printf("recent time is : %s /n", asctime(timenow));  


    printf("Content-Type: text/html \n\n");
    printf("<html>\n");
    printf("<head><meta http-equiv=\"refresh\" content=\"3\"><title>CGI Output</title></head>\n");
    // printf("<head><meta http-equiv=\"content-type\"/><title>CGI Output</title></head>\n");
    
    printf("<body>\n");
    printf("<h1>");
    read_HWclock(fd,buf,addr,2);
    printf("</h1>\n");
    printf("</body>\n"); 
    printf("</html>\n");

    

// printf("<!DOCTYPE html>\n");
// printf("<html lang=\"en\">\n");
// printf("<head>\n");
//     printf("<title>ECharts</title>\n");
//     printf("<!-- 来自百度CDN -->\n");
//     printf("<script src=\"./esl.js\"></script>\n");
// printf("</head>\n");
// printf("<body>\n");
//     printf("<!--#exec cgi=\"cgiProgram.cgi\"-->\n");
//     printf("<div id=\"main2\" style=\"height:600px\" align=\"left\"></div>\n");
//     printf("<script type=\"text/javascript\">\n");
//         // 路径配置
//         printf("require.config({\n");
//             printf("paths:{ \n");
//                 printf("'echarts' : './echarts',\n");
//                 printf("'echarts/chart/bar' : './echarts',\n");
//                 printf("'echarts/chart/line' : './echarts',\n");
//                 printf("'echarts/chart/gauge' : './echarts'\n");
//             printf("}});\n");
         
//         // 使用
//         printf("require([\n");
//                 printf("'echarts',\n");
//                 printf("'echarts/chart/bar', \n");// 使用柱状图就加载bar模块，按需加载
//                 printf("'echarts/chart/line',\n");
//                 printf("'echarts/chart/gauge'],\n");
//             printf("function (ec) {\n");
//                 printf("var myChart2 = ec.init(document.getElementById('main2')); \n");
//                 printf("var option2 = {\n");
//                     printf("tooltip : {formatter: \"{a} <br/>{b} : {c}%\"},\n");
//                     printf("series : [{\n");
//                             printf("name:'燃烧值(kcal)',\n");
//                             printf("type:'gauge',\n");
//                             printf("detail : {formatter:'{value}摄氏度'},\n");
//                             printf("data:[{value: 50, name: ''}]\n");
//                         printf("}]};\n");
//                  printf("myChart2.setOption(option2);});\n");
//     printf("</script>\n");
// printf("</body>\n");
// printf("</html>\n");


    return 0;
    
}
