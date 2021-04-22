import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;

import static java.util.stream.Collectors.joining;

public class WfReport {
  static String home = " ";
  static String away = " ";
  static String score = " ";
  static String half = " ";
  static String competition = " ";
  static String date = " ";
  static String round = " ";
  static String time = " ";
  static String venue = " ";
  static String attend = " ";
  static List<String> evol = new ArrayList();
  static List<String> events = new ArrayList();
  static List<String> hRoster = new ArrayList();
  static List<String> aRoster = new ArrayList();
  static List<String> hSubs = new ArrayList();
  static List<String> aSubs = new ArrayList();
  static Set<String> hScorers = new HashSet();
  static Set<String> aScorers = new HashSet();
  static Map<String, Integer> min = new HashMap();
  static String hCoach = "      ";
  static String aCoach = "      ";
  static String referee = "      ";
  static Map<String, String> eSymbol = new HashMap();

  private static SimpleDateFormat sdf = new SimpleDateFormat("EEEE, dd. MMM yyyy", Locale.US);
  private static SimpleDateFormat idf = new SimpleDateFormat("yyyy-MM-dd");

  public static void main(String args[]) {
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "http://worldfootball.net");
    } catch (IOException e) {
      e.printStackTrace();
    }
    extractReport(doc);
  }

  private static void extractReport(Document doc) {
    Element relevant = doc.selectFirst("#relevant-content");
    Element enavi = relevant.selectFirst("#navi");
    Element header = enavi.selectFirst(".breadcrumb");
    Element content = relevant.selectFirst(".content");
    Elements data = content.select(".data");
    Elements tables = data.select("table");
    Elements stables = data.select(".standard_tabelle");
    extractHeader(header);
    extractInfo(stables.get(0));
    extractGoals(stables.get(1));
    Element elineups = selectTable(tables,"", "", -1);
    extractLineups(elineups);
    Element emanagers = stables.get(stables.size()-2);
    extractManagers(emanagers);
    Element evenue = stables.get(stables.size()-1);
    extractVenue(evenue);
    print();
  }

  public static void print() {
    System.out.println("Home: " + home);
    System.out.println("Away: " + away);
    System.out.println("Score: " + score.replaceAll(" ",""));
    System.out.println("Half-time: " + half);
    System.out.println("Competition: " + competition);
    System.out.println("Date: " + date);
    System.out.println("Round: " + round);
    System.out.println("Time: " + time);
    System.out.println("Venue: " + venue);
    System.out.println("Attendance: " + attend);
    System.out.println("Evolution: " + evol);
    System.out.println("Events: " + events);
    System.out.println("Minutes: " + min);
    System.out.println("Home Roster: " + hRoster);
    System.out.println("Home Subs: " + hSubs);
    System.out.println("Home Coach: " + hCoach);
    System.out.println("Guest Roster: " + aRoster);
    System.out.println("Guest Subs: " + aSubs);
    System.out.println("Guest Coach: " + aCoach);
    System.out.println("Referee: " + referee);
    System.out.println("Lineups: " + serializeLineups());
    System.out.println("Events: " + serializeEvents());
  }

  public static String serializeLineups() {
    StringBuilder sb = new StringBuilder();
    String sc1 = score.replaceAll(" ", "").replaceAll("-", "0");
    sb
        .append(home).append(",")
        .append(away).append(",")
        .append(sc1).append(",")
        .append(String.format("%s@%s", date, time)).append(",A,")
        .append(round).append(",")
        .append(venue).append(",")
        .append(attend).append(", ,");
    for (String s : hRoster) {
      Integer m = 90 + min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=0; i<11-hRoster.size(); i++) {
      sb.append("      :0  ,");
    }
    for (String s : hSubs) {
      Integer m = min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=hSubs.size(); i < 11; i++) {
      sb.append(" :0,");
    }
    sb.append(hCoach).append(",");
    for (String s : aRoster) {
      Integer m = 90 + min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=0; i<11-aRoster.size(); i++) {
      sb.append("      :0  ,");
    }
    for (String s : aSubs) {
      Integer m = min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=aSubs.size(); i < 11; i++) {
      sb.append(" :0,");
    }
    sb.append(aCoach).append(",");
    sb.append(referee).append(", , , , ,");
    return sb.toString();
  }

  public static String serializeEvents() {
    return events.stream().collect(joining(","));
  }

  private static Element selectTable(Elements sections, String cl, String term, int x) {
    List<Element> found = new ArrayList();
    for (Element e : sections) {
      String ec = e.className();
      if (ec!=null && ec.equals(cl)) found.add(e);
    }
    if (x<0 && found.size()>0) return found.get(0);
    if (x>=0 && found.size()>x) return found.get(x);
    return null;
  }

  private static void extractHeader(Element header) {
    Element eline = header.selectFirst("h1");
    String line = eline.childNode(0).toString();
    String parts[] = line.split("»");
    int j = 2;
    for (int i=0; i<parts.length; i++) if (parts[i].contains("Round")) j=i;
    String pround = parts.length > j ? parts[j].trim() : "-1. Round";
    round = pround.split("\\.")[0];
  }

  private static void extractInfo(Element e) {
    if (e == null) return;
    Elements rows = e.select("tr");
    if (rows.size() < 2) return;
    Elements hcols = rows.get(0).select("th");
    Elements tcols = rows.get(1).select("td");
    if (tcols.size() < 3 || hcols.size() < 3) return;
    Element ehome = tcols.get(0).selectFirst("a[href]");
    home = extractLink(ehome, " ", -1, 0);
    Element eaway = tcols.get(2).selectFirst("a[href]");
    away = extractLink(eaway, " ", -1, 0);
    Element edate = hcols.get(1);
    String fdate = edate.childNodeSize() > 0 ? edate.childNode(0).toString().trim() : "0000-00-00";
    String ftime = edate.childNodeSize() > 2 ? edate.childNode(2).toString().trim() : "00:00";
    time = ftime.split(" ")[0];
    date = toIso(fdate);
    Element escore = tcols.get(1).selectFirst(".resultat");
    score = escore.childNode(0).toString().trim();
  }

  private static void extractLineups(Element elineups) {
    if (elineups==null) return;
    boolean ihs = false;
    boolean ias = false;
    Element b = elineups.selectFirst("tr");
    Elements cols = b.select(".standard_tabelle");
    if (cols.size() < 1) return;
    Elements hrows = cols.get(0).select("tr");
    for (Element e : hrows) {
      Element hplink = e.selectFirst("a[href]");
      if (hplink!=null) {
        String hlink = extractLink(hplink, " ", -1, 0);
        if (hlink.length() > 1) {
          int m = extractMins(e);
          if (ihs) {
            hSubs.add(hlink);
            if (m > 0) min.put(hlink, 91-m);
          } else {
            hRoster.add(hlink);
            if (m > 0) deductMin(hlink, m);
          }
        }
        addEvents(e, hlink);
      } else ihs = true;
    }
    Elements arows = cols.size()>1 ? cols.get(1).select("tr") : new Elements();
    for (Element e : arows) {
      Element aplink = e.selectFirst("a[href]");
      if (aplink!=null) {
        String alink = extractLink(aplink, " ", -1, 0);
        if (alink.length() > 1) {
          int m = extractMins(e);
          if (ias) {
            aSubs.add(alink);
            if (m > 0) min.put(alink, 91-m);
          } else {
            aRoster.add(alink);
            if (m > 0) deductMin(alink, m);
          }
        }
        addEvents(e, alink);
      } else ias = true;
    }
  }

  private static int extractMins(Element e) {
    int m = 0;
    int min = 0;
    int mout = 91;
    Elements cols = e.select("td");
    Element emc = cols.size() > 2 ? cols.get(2).selectFirst(".kleine_schrift") : null;
    if (emc != null && emc.childNodeSize() > 0) {
      Element ein = emc.selectFirst(".gruentext");
      Element eout = emc.selectFirst(".rottext");
      if (ein!=null) {
        String sm = ein.childNode(0).toString().replaceAll("'", "");
        min = Integer.valueOf(sm);
        m = min;
      }
      if (eout!=null) {
        String sm = eout.childNode(0).toString().replaceAll("'", "");
        mout = Integer.valueOf(sm);
        m = min > 0 ? min + (91 - mout) : mout;
      }
    }
    return m;
  }

  private static void addEvents(Element e, String plink) {
    if (e==null) return;
    Elements imgs = e.select("img");
    for (Element i : imgs) {
      String t = i.attr("title");
      String a = i.attr("alt");
      if (t.equals("Yellow card") || a.equals("yellow")) {
        Element em = i.nextElementSibling();
        String sm = em!=null && em.childNodeSize() > 0 ?
            em.childNode(0).toString().replaceAll("'","") : "91";
        events.add(String.format("%s#%s", plink, sm));
      } else if (t.equals("Red card") || a.equals("red")
              || t.equals("Second yellow card") || a.equals("Second yellow")) {
        Element em = i.nextElementSibling();
        String sm = em!=null && em.childNodeSize() > 0 ?
            em.childNode(0).toString().replaceAll("'","") : "91";
        events.add(String.format("%s!%s", plink, sm));
        Integer m = Integer.valueOf(sm);
        deductMin(plink, m);
      }
    }
  }

  private static void extractGoals(Element e) {
    if (e==null) return;
    Elements rows = e.select("tr");
    for (Element r : rows) {
      Elements cols = r.select("td");
      if (cols.size() != 2) continue;
      Element scorer = cols.get(1);
      Element plink = scorer.selectFirst("a[href]");
      String pl = extractLink(plink, " ", -1, 0);
      String scm = scorer.childNodeSize() > 2 ?
          scorer.childNode(2).toString().trim().replaceAll("\\.", "") : "0";
      String parts[] = scm.split("/|\\(| ");
      String sm = parts[0].replaceAll("\\.", "").trim();
      boolean isOwn = false;
      boolean isPen = false;
      if (parts.length > 1) {
        isPen = parts[1].toLowerCase().contains("penalty");
        isOwn = parts[1].toLowerCase().contains("own");
      }
      if (isOwn) {
        events.add(String.format("%s`%s", pl, sm));
      } else if (isPen) {
        events.add(String.format("%s\"%s", pl, sm));
      } else {
        events.add(String.format("%s'%s", pl, sm));
      }
    }
  }

  private static void extractManagers(Element e) {
    if (e==null) return;
    Elements rows = e.select("tr");
    if (rows.size() < 1) return;
    Elements cols = rows.get(0).select("td");
    Element clink;
    if (cols.size() < 2) cols = rows.get(0).select("th");
    if (cols.size() > 0) {
      Elements pars = cols.get(0).select("p");
      if (pars.size()>0) {
        clink = pars.get(0).selectFirst("a[href]");
      } else {
        clink = cols.get(0).selectFirst("a[href]");
      }
      hCoach = extractLink(clink, "      ", -1, 0);
    }
    if (cols.size() > 1) {
      Elements pars = cols.get(1).select("p");
      if (pars.size()>0) {
        clink = pars.get(0).selectFirst("a[href]");
      } else {
        clink = cols.get(1).selectFirst("a[href]");
      }
      aCoach = extractLink(clink, "      ", -1, 0);
    }
  }

  private static void extractVenue(Element e) {
    if (e==null) return;
    Elements rows = e.select("tr");
    for (Element r : rows) {
      Elements cols = r.select("td");
      if (cols.size() <3) continue;
      Element mid = cols.get(1);
      Element eimg = mid.selectFirst("img");
      if (eimg == null) continue;
      String png = eimg.attr("src");
      String tit = eimg.absUrl("title");
      if (png.contains("stadion.png") || tit.contains("tadium")) {
        Element estad = cols.get(2).selectFirst("a[href]");
        venue = extractLink(estad, " ", -1, 0);
      } else if (png.contains("schiedsrichter.png") || tit.equals("Referee")) {
        Element eref = cols.get(2).selectFirst("a[href]");
        referee = extractLink(eref, "      ", -1, 0);
      } else if (png.contains("zuschauer.png") || tit.contains("ttendance")) {
        attend = cols.get(2).childNode(0).toString().trim()
            .replaceAll(",", "").replaceAll("\\.", "");
      }
    }
  }

  private static String extractLink(Element n, String def, int start, int end) {
    if (n==null) return def;
    String link = n.attr("href");
    List<String> parts = Arrays.asList(link.split("/"));
    int np = parts.size();
    if (start < 0) {
      return parts.subList(np + start, np + end).stream().collect(joining("/"));
    }
    else {
      return parts.subList(start, end).stream().collect(joining("/"));
    }
  }

  private static void deductMin(String name, int minsub) {
    Integer minp = getMin(name);
    Integer rem = 91 - minsub;
    if (rem <= 0) rem = 1;
    min.put(name, minp - rem);
  }

  private static Integer getMin(String s) {
    return min.getOrDefault(s, 0);
  }

  private static String toIso(String s) {
    try {
      Date d = sdf.parse(s);
      String iso = idf.format(d);
      return iso;
    } catch (ParseException e) {
      return "0000-00-00";
    }
  }

}
