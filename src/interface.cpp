#include "interface.h"

interface::interface()
{

}

void interface::read_gcode_file(std::string filename){

enum g_type {
    G0=0,
    G1=1,
    G2=2,
    G3=3
};

//https://github.com/dillonhuff/gpr/
using namespace gpr;

std::ifstream t(filename.c_str());
std::string file_contents((std::istreambuf_iterator<char>(t)),
                          std::istreambuf_iterator<char>());

gcode_program p = parse_gcode(file_contents);

for(int i=0; i<p.num_blocks(); i++){
    //std::cout<<"gcode line chunck size:"<<p.get_block(i).size()<<std::endl; // Text editor line +1.

    for(int chunk=0; chunk<p.get_block(i).size(); chunk++){

        /*
        std::cout<<"chunk data: "<<p.get_block(i).get_chunk(chunk)<<std::endl; // Text editor line +1.
        if(p.get_block(i).get_chunk(chunk).tp()==CHUNK_TYPE_WORD_ADDRESS){ // tp=type
            std::cout<<"chunk word id: "<<p.get_block(i).get_chunk(chunk).get_word()<<std::endl; // the chunk Id.

            block b=p.get_block(i);
            if(b.get_chunk(chunk).get_address().tp()==ADDRESS_TYPE_DOUBLE){
                std::cout<<"chunk double value: "<<b.get_chunk(chunk).get_address().double_value()<<std::endl;
            }

            if(b.get_chunk(chunk).get_address().tp()==ADDRESS_TYPE_INTEGER){
                std::cout<<"chunk integer value: "<<b.get_chunk(chunk).get_address().int_value()<<std::endl;
            }

            if(b.get_chunk(chunk).tp()==CHUNK_TYPE_COMMENT){
                std::cout<<"chunk comment: "<<b.get_chunk(chunk).get_comment_text()<<std::endl; // the chunk Id.
            }
        }
        */

        // Find the character : g,G
        char a='0';
        if(p.get_block(i).get_chunk(chunk).tp()==CHUNK_TYPE_WORD_ADDRESS){ // tp=type
            a=p.get_block(i).get_chunk(chunk).get_word();
        }
        char axisletter;
        int gtype=11111;
        double X=0,Y=0,Z=0,I=0,J=0,F=0;

        if(a=='G' || a=='g'){
            // std::cout<<"G found"<<std::endl;
            // Find 0,1,2,3
            gtype=p.get_block(i).get_chunk(chunk).get_address().int_value();
            // std::cout<<"G type:"<<gtype<<std::endl;

            if(gtype==0){
                // std::cout<<"G0, draw a rapid"<<std::endl;

                for(int j=chunk+1; j<p.get_block(i).size(); j++){
                    // Get the xyz values.
                    axisletter=p.get_block(i).get_chunk(j).get_word();
                    if(axisletter=='X' || axisletter=='x'){
                        X=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='Y' || axisletter=='y'){
                        Y=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='Z' || axisletter=='z'){
                        Z=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                }
                std::cout<<"g0 x:"<<X<<" y:"<<Y<<" z:"<<Z<<" f:"<<F<<std::endl;
            }
            if(gtype==1){
                // std::cout<<"G1, draw a line"<<std::endl;

                for(int j=chunk+1; j<p.get_block(i).size(); j++){
                    // Get the xyz values.
                    axisletter=p.get_block(i).get_chunk(j).get_word();
                    if(axisletter=='X' || axisletter=='x'){
                        X=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='Y' || axisletter=='y'){
                        Y=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='Z' || axisletter=='z'){
                        Z=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='F' || axisletter=='f'){
                        F=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                }
                std::cout<<"g1 x:"<<X<<" y:"<<Y<<" z:"<<Z<<" f:"<<F<<std::endl;
            }
            if(gtype==2){
                // std::cout<<"G2, draw a cw arc"<<std::endl;

                for(int j=chunk+1; j<p.get_block(i).size(); j++){
                    // Get the xyz values.
                    axisletter=p.get_block(i).get_chunk(j).get_word();
                    if(axisletter=='X' || axisletter=='x'){
                        X=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='Y' || axisletter=='y'){
                        Y=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='Z' || axisletter=='z'){
                        Z=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='I' || axisletter=='i'){
                        I=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='J' || axisletter=='j'){
                        J=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='F' || axisletter=='f'){
                        F=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                }
                std::cout<<"g2 x:"<<X<<" y:"<<Y<<" z:"<<Z<<" i:"<<I<<" j:"<<J<<" f:"<<F<<std::endl;
            }
            if(gtype==3){
                // std::cout<<"G3, draw a ccw arc"<<std::endl;

                for(int j=chunk+1; j<p.get_block(i).size(); j++){
                    // Get the xyz values.
                    axisletter=p.get_block(i).get_chunk(j).get_word();
                    if(axisletter=='X' || axisletter=='x'){
                        X=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='Y' || axisletter=='y'){
                        Y=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='Z' || axisletter=='z'){
                        Z=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='I' || axisletter=='i'){
                        I=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='J' || axisletter=='j'){
                        J=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                    if(axisletter=='F' || axisletter=='f'){
                        F=p.get_block(i).get_chunk(j).get_address().double_value();
                    }
                }
                std::cout<<"g3 x:"<<X<<" y:"<<Y<<" z:"<<Z<<" i:"<<I<<" j:"<<J<<" f:"<<F<<std::endl;
            }
        }
    }
}

std::cout<<" "<<std::endl; // the chunk Id.


}
