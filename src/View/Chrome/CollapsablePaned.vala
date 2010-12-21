using Gdk;

//Why in gtk one might wonder, well one day i hope to throw this file in a library, currently library support in vala is sketchy at best.

namespace Gtk{
    public enum CollapseMode{
        NONE=0,
        LEFT=1, TOP=1, FIRST=1,
        RIGHT=2, BOTTOM=2, LAST=2
    }

    public class CollapsablePaned : HPaned{
        private int saved_state = 10;
        private uint last_click_time = 0;
        
        public CollapseMode collapse_mode = CollapseMode.NONE;
        //public signal void shrink(); //TODO: Make the default action overwriteable
        //public new signal void expand(int saved_state); //TODO same
    
        public CollapsablePaned(){
            //events |= EventMask.BUTTON_PRESS_MASK;
            
            button_press_event.connect(detect_toggle);
        }
    
        private bool detect_toggle( Gdk.EventButton event )
        {
            if(collapse_mode == CollapseMode.NONE){
                return false;
            }
        
            if(event.time < (last_click_time + Gtk.Settings.get_default().gtk_double_click_time) && event.type != EventType.2BUTTON_PRESS ){
                return true;
            }        
        
            if( 
              Gdk.Window.at_pointer( null, null ) == this.get_handle_window() 
              && event.type == EventType.2BUTTON_PRESS 
            ){
                accept_position();
                
                var current_position = get_position();
                
                if(collapse_mode == CollapseMode.LAST){ 
                    current_position = (max_position - current_position); // change current_position to be relative
                }
                
                int requested_position;                
                if( current_position == 0 ){
                    Log.println( Log.Level.INFO, "[CollapsablePaned] expand");
                    
                    requested_position = saved_state;
                }
                else{
                    saved_state = current_position;
                    Log.println( Log.Level.INFO, "[CollapsablePaned] shrink" );
                    
                    requested_position = 0;
                }
                
                if(collapse_mode == CollapseMode.LAST){
                    requested_position = (max_position - requested_position); // change requeste_position back to be non-relative
                }
                
                set_position(requested_position);
                
                return true;
            }
            
            last_click_time = event.time;
            
            return false;
        }
    }
    
    public class HCollapsablePaned : CollapsablePaned{
        public HCollapsablePaned(){
            set_orientation(Orientation.HORIZONTAL);
        }
    }
    
    public class VCollapsablePaned : CollapsablePaned{
        public VCollapsablePaned(){
            set_orientation(Orientation.VERTICAL);
        }
    }
}