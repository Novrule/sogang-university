package homework3;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.util.List;

public class assign3_20191282 extends Frame implements ActionListener {
    List<Ball> balls = new ArrayList<>();
    private BounceCanvas canvas;

    public assign3_20191282() {
    	setTitle("BallThread");
    	
        canvas = new BounceCanvas(balls);
        add("Center", canvas);
        
        Panel p = new Panel();
        Button startButton = new Button("Start");
        Button closeButton = new Button("Close");

        p.add(startButton);
        p.add(closeButton);
        
        startButton.addActionListener(this);
        closeButton.addActionListener(this);
        
        add("South", p);
    }

    public void actionPerformed(ActionEvent evt) {
        if ("Start".equals(evt.getActionCommand())) {
            for (int i = 0; i < 5; i++) {
                Ball ball = new Ball(canvas, Utils.getRandomSpeed(), Utils.getRandomSpeed(), balls, 16);
                
                synchronized (balls) {
                    balls.add(ball);
                }
                ball.start(); // 스레드 시작
            }
        } else if ("Close".equals(evt.getActionCommand())) {
            System.exit(0); // 시스템 종료
        }
    }

    public static void main(String[] args) {
        Frame f = new assign3_20191282();
        WindowDestroyer listener = new WindowDestroyer();
        
        f.setSize(1200, 800);
        f.addWindowListener(listener);
        f.setVisible(true);
    }
}

class BounceCanvas extends Canvas {
    private List<Ball> balls;
    private Image offscreenImage;
    private Graphics offscreenGraphics;

    public BounceCanvas(List<Ball> balls) {
        this.balls = balls;
    }

    @Override
    public void paint(Graphics g) {
        if (offscreenImage == null) {
            offscreenImage = createImage(getWidth(), getHeight());
            offscreenGraphics = offscreenImage.getGraphics();
        }

        offscreenGraphics.setColor(getBackground());
        offscreenGraphics.fillRect(0, 0, getWidth(), getHeight());

        synchronized (balls) {
            for (Ball b : balls) {
                b.draw(offscreenGraphics);
            }
        }

        g.drawImage(offscreenImage, 0, 0, this);
    }

    @Override
    public void update(Graphics g) {
        paint(g);
    }
}

class Ball extends Thread {
    private Canvas box;
    private int XSIZE;
    private int YSIZE;
    private int x;
    private int y;
    private int dx;
    private int dy;
    private List<Ball> balls;
    private int movesCount = 0;
    private static final int INITIAL_MOVES = 50;

    public Ball(Canvas c, int speedX, int speedY, List<Ball> ballsList, int size) {
        this(c, speedX, speedY, ballsList, size, c.getSize().width / 2 - size / 2, c.getSize().height / 2 - size / 2);
    }

    public Ball(Canvas c, int speedX, int speedY, List<Ball> ballsList, int size, int startX, int startY) {
        box = c;
        XSIZE = size;
        YSIZE = size;
        x = startX;
        y = startY;
        dx = speedX;
        dy = speedY;
        balls = ballsList;
    }

    public void draw(Graphics g) {
        g.setColor(Color.BLACK);
        g.fillOval(x, y, XSIZE, YSIZE);
    }

    public void run() {
        try {
            while (!Thread.interrupted()) {
                move();
                
                if (movesCount > INITIAL_MOVES) {
                    checkCollisionAndResize();
                }
                
                if (XSIZE <= 1 || YSIZE <= 1) {
                    break;
                }
                
                Thread.sleep(5);
            }
        } catch (InterruptedException ignored) {
        } finally {
            synchronized (balls) {
                balls.remove(this);
            }
        }
    }

    public void move() {
        Dimension d = box.getSize();
        
        x += dx;
        y += dy;
        movesCount++;
        
        if (x < 0 || x + XSIZE >= d.width) {
        	dx = -dx;
        }
        
        if (y < 0 || y + YSIZE >= d.height) {
        	dy = -dy;
        }
        
        box.repaint();
    }

    private void checkCollisionAndResize() {
        List<Ball> newBalls = new ArrayList<>();
        
        synchronized (balls) {
            for (Ball b : balls) {
                if (b != this && b.getBounds().intersects(this.getBounds())) {
                    int size_this = this.XSIZE;
                    int size_b = b.XSIZE;

                    newBalls.add(new Ball(box, Utils.getRandomSpeed(), Utils.getRandomSpeed(), balls, size_this / 2, x, y));
                    newBalls.add(new Ball(box, Utils.getRandomSpeed(), Utils.getRandomSpeed(), balls, size_this / 2, x, y));
                    newBalls.add(new Ball(box, Utils.getRandomSpeed(), Utils.getRandomSpeed(), balls, size_b / 2, b.x, b.y));
                    newBalls.add(new Ball(box, Utils.getRandomSpeed(), Utils.getRandomSpeed(), balls, size_b / 2, b.x, b.y));

                    balls.remove(this);
                    balls.remove(b);
                    
                    this.interrupt();
                    b.interrupt();
                    
                    break;
                }
            }
            
            balls.addAll(newBalls);
            newBalls.forEach(Ball::start);
        }
    }

    public Rectangle getBounds() {
        return new Rectangle(x, y, XSIZE, YSIZE);
    }
}

class Utils {
    public static int getRandomSpeed() {
        return (int) (Math.random() * 10 + 1) * (Math.random() > 0.5 ? 1 : -1);
    }
}

class WindowDestroyer extends WindowAdapter {
    @Override
    public void windowClosing(WindowEvent e) {
        System.exit(0);
    }
}
